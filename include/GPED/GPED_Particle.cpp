#include <assert.h>
#include "GPED_Particle.h"

using namespace GPED;

GPED::GPEDParticle::GPEDParticle()
	: m_position(glm::vec3(0.0)), m_velocity(glm::vec3(0.0)), m_acceleration(glm::vec3(0.0)),
	m_forceAccum(glm::vec3(0.0)),
	damping(1.0), inverseMass(1.0)
{

}

void GPEDParticle::integrate(real duration)
{
	// 조건이 false면 런타임 때 에러 배출
	assert(duration > 0.0);
	
	// Update linear position
	m_position += m_velocity * duration;

	// Work out the acceleration from the force
	glm::vec3 resultingAcc = m_acceleration;
	resultingAcc += m_forceAccum * inverseMass;

	// Update linear velocity from the acceleration
	m_velocity += resultingAcc * duration;

	// Impose drag
	m_velocity *= real_pow(damping, duration);

	clearAccumulator();
}

void GPEDParticle::setDamping(const real damping)
{
	GPEDParticle::damping = damping;
}

real GPEDParticle::getDamping() const
{
	return damping;
}

void GPEDParticle::setInverseMass(const real inverseMass)
{
	GPEDParticle::invereseMass = inverseMass;
}

real GPEDParticle::getInverseMass() const
{
	return inverseMass;
}

bool GPED::GPEDParticle::hasFiniteMass() const
{
	return inverseMass >= 0.0;
}

void GPEDParticle::setMass(const real Mass)
{
	assert(Mass != 0);
	GPEDParticle::inverseMass = real(1.0) / Mass;
}

real GPEDParticle::getMass() const
{
	if (inverseMass == 0)
		return REAL_MAX;
	else
		return real(1.0) / inverseMass;
}

void GPEDParticle::setPosition(const glm::vec3 position)
{
	GPEDParticle::getPosition = position;
}

void GPEDParticle::setPosition(const real x, const real y, const real z)
{
	GPEDParticle::getPosition = glm::vec3(x, y, z);
}

glm::vec3 GPEDParticle::getPosition() const
{
	return m_position;
}

void GPEDParticle::setVelocity(const glm::vec3 velocity)
{
	GPEDParticle::setVelocity = velocity;
}

void GPEDParticle::setVelocity(const real x, const real y, const real z)
{
	GPEDParticle::setVelocity = glm::vec3(x, y, z);
}

glm::vec3 GPEDParticle::getVeclocity() const
{
	return m_velocity;
}

void GPEDParticle::setAcceleration(const glm::vec3 acceleration)
{
	GPEDParticle::getAcceleration = acceleration;
}

void GPEDParticle::setAcceleration(const real x, const real y, const real z)
{
	GPEDParticle::getAcceleration = glm::vec3(x, y, z);
}

glm::vec3 GPEDParticle::getAcceleration() const
{
	return m_acceleration;
}

void GPEDParticle::clearAccumulator()
{
	m_forceAccum = glm::vec3(real(0.0));
}

void GPEDParticle::addForce(const glm::vec3 & force)
{
	m_forceAccum += force;
}