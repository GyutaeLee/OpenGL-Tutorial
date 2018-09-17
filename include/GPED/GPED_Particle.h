#ifndef __GPED_PARTICLE_H__
#define __GPED_PARTICLE_H__

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "GPED_Precision.h"

namespace GPED
{
	class GPEDParticle
	{
	protected:
		glm::vec3 m_position;
		glm::vec3 m_velocity;
		glm::vec3 m_acceleration;

		glm::vec3 m_forceAccum;

		real damping;
		real inverseMass;
	public:
		GPEDParticle();

		void integrate(real duration);

		void setDamping(const real damping);
		real getDamping() const;

		void setInverseMass(const real inverseMass);
		real getInverseMass() const;
		bool hasFiniteMass() const;

		void setMass(const real Mass);
		real getMass() const;
	
		void setPosition(const glm::vec3 position);
		void setPosition(const real x, const real y, const real z);
		glm::vec3 getPosition() const;

		void setVelocity(const glm::vec3 velocity);
		void setVelocity(const real x, const real y, const real z);
		glm::vec3 getVeclocity() const;
	
		void setAcceleration(const glm::vec3 acceleration);
		void setAcceleration(const real x, const real y, const real z);
		glm::vec3 getAcceleration() const;

		void clearAccumulator();
		void addForce(const glm::vec3& force);

		/*
		이것은 데모를 위한 테스트 기능이다.
		이 기능을 실제 어플리케이션에 넣지말아라.
		너가 OOP 패턴을 원한다면.
		*/
		glm::vec3* getPositionPointer()
		{
			return &m_position;
		}
	};
}
#endif