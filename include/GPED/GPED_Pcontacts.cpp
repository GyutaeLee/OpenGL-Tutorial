#include "GPED_Pcontacts.h"
using namespace GPED;

void GPED::ParticleContact::resolve(real duration)
{
	resolveVelocity(duration);
	resolveInterpenetration(duration);
}

real GPED::ParticleContact::calculateSepartingVelocity() const
{
	glm::vec3 relativeVelocity = particle[0]->getVelocity();
	if (particle[1]) relativeVelocity -= particle[1]->getVeclocity();
	return glm::dot(relativeVelocity, contactNormal);
}

void GPED::ParticleContact::resolveVelocity(real duration)
{
	// contact 방향의 속도를 구한다
	real separatingVelocity = calculateSepartingVelocity();

	// 해결할 필요가 있는지 없는지 확인한다
	if (separatingVelocity > 0)
	{
		//contact가 분리되어있거나 고정되어있어서 아무런 것도 필요하지 않다
		return;
	}

	// 새롭게 분리된 속도를 계산한다
	real newSepVelocity = -separatingVelocity * restitution;

	// 가속만으로 속도 생성을 점검해라
	glm::vec3 accCausedVelocity = particle[0]->getAcceleration();
	if (particle[1]) accCausedVelocity -= particle[1]->getAcceleration();
	real accCausedSepVelocity = glm::dot(accCausedVelocity, contactNormal) * duration;

	// 가속도 상승 때문에 closing 속도가 있다면,
	// 새로운 분리 속도에서 제거한다
	if (accCausedSepVelocity < 0)
	{
		newSepVelocity += restitution * accCausedSepVelocity;

		// 우리가 제거 할 곳보다 더 많이 옮기지 않았는지 확인해라
		if (newSepVelocity < 0) newSepVelocity = 0;
	}

	real deltaVelocity = newSepVelocity - separatingVelocity;

	// 역 질량에 비례하여 각 물체에 속도 변화를 적용한다
	// (즉, 역 질량이 낮은 물체는 속도 변화가 적음)
	real totalInverseMass = particle[0]->getInverseMass();
	if (particle[1]) totalInverseMass += particle[1]->getInverseMass();

	// 모든 입자가 무한 질량을 갖는다면, 충격은 효과가 없다
	if (totalInverseMass <= 0) return;

	// 적용할 충돌을 계산해라
	real impulse = deltaVelocity / totalInverseMass;
	if (particle[0] && particle[1])
	{
		impulse *= particle[0]->getInverseMass()*particle[1]->getInverseMass();
	}

	// 역 질량의 단위 당 충돌 양을 찾는다
	glm::vec3 impulsePerIMass = contactNormal * impulse;

	// 충격을 가해라: 그들은 접촉의 방향으로 적용되고 역 질량에 비례한다
	particle[0]->setVelocity
	(
		particle[0]->getVeclocity() + impulsePerIMass * particle[0]->getInverseMass()
	);
	if (particle[1])
	{
		// 입자 1은 반대 방향으로 간다
		particle[1]->setVelocity
		(
			particle[1]->setVelocity() + impulsePerIMass * -particle[0]->getInverseMass()
		);
	}
}

void GPED::ParticleContact::resolveInterpenetration(real duration)
{
	// 침투가 없다면 이 단계를 건너뛴다
	if (penetration <= 0) return;

	// 각 물체의 움직임은 역 질량을 기반으로 한다
	real totalInverseMass = particle[0]->getInverseMass();
	if (particle[1]) totalInverseMass += particle[1]->getInverseMass();

	// 모든 입자가 무한 질량을 가지면, 아무것도 하지 않는다
	if (totalInverseMass <= 0) return;

	// 역 질량의 단위당 침투 분해양을 파악한다
	glm::vec3 movePerIMass = contactNormal * (penetration / totalInverseMass);

	// 이동량을 계산한다
	particleMovement[0] = movePerIMass * particle[0]->getInverseMass();
	if (particle[1])
		particleMovement[1] = movePerIMass * -particle[1]->getInverseMass();
	else
		particleMovement[1] = glm::vec3(0.0);

	// 침투 해상도 적용
	particle[0]->setPosition
	(
		particle[0]->getPosition() + particleMovement[0]
	);
	if (particle[1])
	{
		particle[1]->setPosition
		(
			particle[1]->getPosition() + particleMovement[1]
		);
	}
}

GPED::ParticleContactResolver::ParticleContactResolver(unsigned iterations)
	:iterations(iterations), iterationUsed(0)
{ }

void GPED::ParticleContactResolver::setIterations(unsigned iterations)
{
	ParticleContactResolver::iterations = iterations;
}

void GPED::ParticleContactResolver::resolveContacts(ParticleContact* contactArray, unsigned numContacts, real duration)
{
	unsigned i;

	iterationUsed = 0;
	while (iterationUsed < iterations)
	{
		// 가장 큰 closing 속도로 접촉을 찾아라
		real max = REAL_MAX;
		unsigned maxIndex = numContacts;
		for (i = 0; i < numContacts; ++i)
		{
			real sepVel = contactArray[i].calculateSepartingVelocity();
			if (sepVel < max && (sepVel < 0 || contactArray[i].penetration>0))
			{
				max = sepVel;
				maxIndex = i;
			}
		}

		// 해결할만한 가치가 있는 것이 있는지 본다
		if (maxIndex == numContacts) break;

		// 이 contact를 해결한다
		contactArray[maxIndex].resolve(duration);

		// 모든 파티클에 대한 상호 침투를 업데이트한다
		glm::vec3* move = contactArray[maxIndex].particleMovement;
		for (i = 0; i < numContacts; ++i)
		{
			if (contactArray[i].particle[0] == contactArray[maxIndex].particle[0])
			{
				contactArray[i].penetration -= glm::dot(move[0], contactArray[i].contactNormal);
			}
			else if (contactArray[i].particle[0] == contactArray[maxIndex].particle[1])
			{
				contactArray[i].penetration -= glm::dot(move[1], contactArray[i].contactNormal);
			}

			if (contactArray[i].particle[1])
			{
				if (contactArray[i].particle[1] == contactArray[maxIndex].particle[0])
				{
					contactArray[i].penetration += glm::dot(move[0], contactArray[i].contactNormal);
				}
				else if (contactArray[i].particle[1] == contactArray[maxIndex].particle[1])
				{
					contactArray[i].penetration += glm::dot(move[1], contactArray[i].contactNormal);
				}
			}
		}
		++iterationUsed;
	}
}