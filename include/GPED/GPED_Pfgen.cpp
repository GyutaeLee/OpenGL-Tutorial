#include "GPED_Pfgen.h"

using namespace GPED;

GPED::ParticleGravity::ParticleGravity(const glm::vec3 & gravity)
{
	ParticleGravity::gravity = gravity;
}

void GPED::ParticleGravity::updateForce(GPEDParticle* particle, real duration)
{
	// 무한 질량이 없는지 확인해라
	if (!particle->hasFiniteMass()) return;

	// 파티클에 mass-scaled 적용한다
	particle->addForce(gravity*particle->getMass());
}

GPED::ParticleDrag::ParticleDrag(real k1, real k2)
{
	ParticleDrag::k1 = k1;
	ParticleDrag::k2 = k2;
}

void GPED::ParticleDrag::updateForce(GPEDParticle * particle, real duration)
{
	glm::vec3 force = particle->getVelocity();

	// 총 저항 계수를 계산해라
	real dragCoeff = glm::length(force);
	dragCoeff = k1 * dragCoeff + k2 * dragCoeff * dragCoeff;

	// 최종 힘을 계산하고 적용해라
	force = glm::normalize(force);
	force *= -dragCoeff;
	particle->addForce(force);
}

GPED::ParticleSpring::ParticleSpring(GPEDParticle* other, real springConstant, real restLength)
{
	ParticleSpring::other = other;
	ParticleSpring::springConstant = springConstant;
	ParticleSpring::restlength = restlength;
}

void GPED::ParticleSpring::updateForce(GPEDParticle* particle, real duration)
{
	// 스프링의 벡터를 계산한다
	glm::vec3 force = particle->getPosition();
	force -= *anchor;

	// 힘의 크기를 계산한다
	real magnitude = glm::length(force);
	magnitude = real_abs(magnitude - restLength); // 찬행이형 말로는 cable rope bridge를 구현하고 싶으면 이 라인을 수정해야한다고 생각한다고 한다
	magnitude *= springConstant;

	// 마지막 힘을 계산하고 적용해라
	force = glm::normalize(force);
	force *= -magnitude;
	particle->addForce(force);
}

GPED::ParticleBungee::ParticleBungee(GPEDParticle* other, real springConstant, real restLength)
{
	ParticleBungee::other = other;
	ParticleBungee::springConstant = springConstant;
	ParticleBungee::restLength = restLength;
}

void GPED::ParticleBungee::updateForce(GPEDParticle* particle, real duration)
{
	// 스프링의 벡터를 계산한다
	glm::vec3 force = particle->getPosition();
	force -= other->getPosition();

	// 번지가 압축되어 있는지 확인해라
	real magnitude = glm::length(force);
	if (magnitude <= restLength) return;

	// 힘의 크기를 계산해라
	magnitude = springConstant * (restLength - magnitude);

	// 마지막 힘을 계산하고 적용해라
	force = glm::normalize(force);
	force *= -magnitude;
	particle->addForce(force);
}

void GPED::ParticleAnchoredBungee::updateForce(GPEDParticle* particle, real duration)
{
	// 스프링의 벡터를 계산한다
	glm::vec3 force = particle->getPosition();
	force -= *anchor;
	
	// 힘의 크기를 계산한다
	real magnitude = glm::length(force);
	if (magnitude < restLength) return;

	magnitude = springConstant * (magnitude - restLength);

	// 마지막 힘을 계산하고 적용한다
	force = glm::normalize(force);
	force *= -magnitude;
	particle->addFroce(force);
}

GPED::ParticleBuoyancy::ParticleBuoyancy(real maxDepth, real volume, real waterHeight, real liquidDensity)
{
	ParticleBuoyancy::maxDepth = maxDepth;
	ParticleBuoyance::volume = volume;
	ParticleBuoyance::waterHeight = waterHeight;
	ParticleBuoyancy::liquidDensity = liquidDensity;
}

void GPED::ParticleBuoyany::updateForce(GPEDParticle* particle, real duration)
{
	// 잠수 깊이를 계산한다
	real depth = particle->getPosition().y;

	// 물 밖으로 나갔는지 점검한다
	if (depth >= waterHeight + maxDepth) return;
	glm::vec3 force(0.0);

	// 최대 깊이에 있는지 아닌지 검사한다
	if (depth <= waterHeight - maxDepth)
	{
		force.y = liquidDensity * volume;
		particle->addForce(force);
		return;
	}

	// 그렇지 않다면 우리는 부분족으로 물속에 잠긴 것이다
	force.y = liquidDensity * volume*(depth - maxDepth - waterHeight) / 2 * maxDepth;
	particle->addForce(force);
}

void GPED::ParticleForceRegistry::add(GPEDParticle* particle, ParticleForceGenerator* fg)
{
	registrations.push_back(ParticleForceRegistration{ particle,fg });
}

void GPED::ParticleForceRegistry::remove(GPEDParticle* particle, ParticleForceGenerator* fg)
{
	Registry::iterator i = registrations.begin();
	for (; i != registrations.end(); ++i)
		if (i->particle == particle && i->fg == fg)
			registrations.erase(i);
}

void GPED::ParticleForceRegistry::clear()
{
	registrations.clear();
}

void GPED::ParticleForceRegistry::updateForces(real duration)
{
	Registry::iterator i = registrations.begin();
	for (; i != registrations.end(); ++i)
		i->fg->updateForce(i->particle, duration);
}