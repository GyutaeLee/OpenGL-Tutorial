#ifndef __GPED_PFGEN_H__
#define __GPED_PFGEN_H__

/*
힘 생성기(force generator)가 힘을 하나에만 추가할지 더 많은 파티클에 추가할지를 묻는다
*/

#include "GPED_Particle.h"
#include <vector>

namespace GPED
{
	class ParticleForceGenerator
	{
		/*
		주어진 파티클에 적용된 힘을 계산하고 업데이트하기 위해
		인터페이스의 구현에서 오버로드해라
		*/
		virtual void updateForce(GPEDParticle* particle, real duration) = 0;
	};

	/*
	중력을 가하는 힘 생성기
	하나의 인스턴스는 여러 개의 입자에 사용할 수 있다
	*/
	class ParticleGravity : public ParticleForceGenerator
	{
		/*	중력 가속도를 유지한다 */
		glm::vec3 gravity;
	public:
		/* 주어진 가속도로 발생기를 생성한다 */
		ParticleGravity(const glm::vec3& gravity);

		/* 주어진 파티클에 중력 힘을 적용한다 */
		virtual void updateForce(GPEDParticle* particle, real duration);
	};

	/*
	힘 발생기는 마찰력을 적용한다
	하나의 인스턴스는 여러 파티클에 이용될 수 있다
	*/
	class ParticleDrag : public ParticleForceGenerator
	{
		/* 속도 저항 계수를 보유 */
		real k1;

		/* 속도 제곱 저항 계수를 보유 */
		real k2;

	public:
		/* 주어진 계수에 맞게 발생기를 생성한다 */
		ParticleDrag(real k1, real k2);

		/* 주어진 파티클에 마찰력을 적용한다 */
		virtual void updateForce(GPEDParticle* particle, real duration);
	};

	/*
	스프링의 힘을 가하는 힘 발생기
	*/
	class ParticleSpring : public ParticleForceGenerator
	{
		/* 스프링의 다른 끝에 있는 입자 */
		GPEDParticle* other;

		/* 스프링 상수를 유지한다 */
		real springConstant;

		/* 스프링의 나머지 길이를 유지한다 */
		real restlength;

	public:
		/* 주어진 매개변수에서 새로운 스프링을 생성한다 */
		ParticleSpring(GPEDParticle* other, real springConstant, real restLength);

		/* 주어진 파티클에 스프링의 힘을 적용한다 */
		virtual void updateForce(GPEDParticle* particle, real duration);
	};

	/*
	한쪽 끝이 공간의 고정 점에 부착되는 스프링 힘을 적용하는 힘 발생기
	*/
	class ParticleAnchoredSpring : public ParticleForceGenerator
	{
	protected:
		/* 스프링 고정 단의 위치 */
		glm::vec3* anchor;

		/*스프링 상수를 유지한다 */
		real springConstant;

		/* 스프링의 나머지 길이를 유지한다 */
		real restLength;

	public:
		/* 주어진 매개변수에서 새로운 스프링을 생성한다 */
		ParticleAnchoredSpring(glm::vec3* anchor, real springConstant, real restLength);

		/* 주어진 파티클에 스프링의 힘을 적용한다 */
		virtual void updateForce(GPEDParticle* particle, real duration);

		/* 앵커 포인트 검색 */
		const glm::vec3* getAnchor() const { return anchor; }
	};

	/*
	확장되었을 때만 스프링 힘을 적용하는 힘 발생기
	*/
	class ParticleBungee : public ParticleForceGenerator
	{
		/* 스프링의 다른 끝에 있는 파티클 */
		GPEDParticle* other;

		/* 스프링 상수를 보유한다 */
		real springConstant;

		/*
		힘을 발생시키기 시작한 지점에서 번지의 길이를 유지한다
		*/
		real restLength;

	public:
		/* 주어진 매개변수에서 새로운 번지를 생성한다 */
		ParticleBungee(GPEDParticle* other, real springConstant, real restLength);

		/* 주어진 파티클에 스프링 힘을 적용한다 */
		virtual void updateForce(GPEDParticle* particle, real duration);
	};

	/*
	번지 힘을 적용하는 힘 발생기.
	한쪽 끝은 공간의 고정 점에 연결된다
	*/
	class ParticleAnchoredBungee : public ParticleAnchoredSpring
	{
	public:
		/* ParticleAnchoredSpring의 초기화 프로그램을 사용해라 */
		using ParticleAnchoredSpring::ParticleAnchoredSpring;

		/* 주어진 파티클에 스프링 힘을 적용해라 */
		virtual void updateForce(GPEDParticle* particle, real duration);
	};

	/*
	XZ 평면에 평핸한 액체 평면에 대해 부력을 적용하는 힘 발생기
	*/
	class ParticleBuoyancy : public ParticleForceGenerator
	{
		/*
		물체가 최대 부력을 발생시키기 전에 물체의 최대 침수 깊이
		*/
		real maxDepth;

		/*
		물체의 부피
		*/
		real volume;

		/*
		y = 0 위의 수면 평면의 높이
		평면은 XZ 평면에 평행하다
		*/
		real waterHeight;

		/*
		액체의 밀도
		순수한 물의 밀도는 1000kg/m^3이다
		*/
		real liquidDensity;

	public:
		/* 주어진 매개 변수로 새로운 부력을 생성한다 */
		ParticleBuoyancy(real maxDepth, real volume, real waterHeight, real liquidDensity = 1000.0f);

		/* 주어진 입자에 부력을 적용한다 */
		virtual void updateForce(GPEDParticle* particle, real duration);
	};

	/*
	모든 힘 생성기와 그들이 적용하는 파티클을 보유한다
	*/
	class ParticleForceRegistry
	{
	protected:
		/*
		주어진 힘 생성기가 주어진 입자에 적용되도록 등록한다
		*/
		void add(GPEDParticle* particle, ParticleForceGenerator* fg);

		/*
		지정된 등록 쌍을 레지스트리에서 제거한다
		페어가 등록되어 있지 않은 경우, 이 메소드는 아무것도 하지 않는다
		*/
		void remove(GPEDParticle* particle, ParticleForceGenerator* fg);

		/*
		레지스트리의 모든 등록을 지운다
		이렇게하면 파티클이나 힘 생성기 자체는 삭제되지 않으며 단지 연결 기록만 삭제된다
		*/
		void clear();

		/*
		모든 힘 생성기를 호출해 해당 입자의 힘을 업데이트한다
		*/
		void updateForces(real duration);
	};

}

#endif