#ifndef __GPED_PCONTACTS_H__
#define __GPED_PCONTACTS_H__

#include "GPED_Particle.h"

namespace GPED
{
	/*
	Contact는 두 객체의 나타냅니다 (이 경우 두 개의 입자를 나타내는 ParticleContact)
	상호작용을 해결하면 상호 침투가 제거되고 서로 밀착하도록 충분한 충동을 준다.
	충돌하는 몸체도 리바운드될 수 있다.
	
	Contact에는 호출 할 수 있는 기능이 없으며 Contact의 디테일만 보유한다.
	Contact 세트를 해결하려면 Particle contact resolver 클래스를 사용해라.
	*/
	class ParticleContact
	{
		/*
		Contact resolver 객체는 contact를 세트하고 영향을 주기 위해
		contact의 접근 권한이 필요하다.
		*/
		friend class ParticleContactResolver;

	public:
		/*
		Contact와 관련된 파티클을 보유한다.
		두 번째 항목이 scenery와 관련된 contact인 경우 NULL이 될 수 있다.
		*/
		GPEDParticle* particle[2];

		/*
		contact의 정상적인 반발 게수를 유지한다.
		*/
		real restitution;

		/*
		world 좌표계에서 contact의 방향을 유지한다.
		*/
		glm::vec3 contactNormal;

		/*
		contact에서 침투의 깊이를 유지한다.
		*/
		real penetration;

		/*
		상호 침투 해결이 발생하는 동안 제거된 각 파티클의 양을 유지한다.
		*/
		glm::vec3 particleMovement[2];

	protected:
		/*
		속도와 상호 침투를 위해 이 contact를 해결한다.
		*/
		void resolve(real duration);

		/*
		contact에서 분리 속도를 계산한다.
		*/
		real calculateSepartingVelocity() const;

	private:
		/*
		이 충돌의 충격 계산을 조절한다.
		*/
		void resolveVelocity(real duration);

		/*
		이 contact의 상호 작용 해결을 조절한다.
		*/
		void resolveInterpenetration(real duration);
	};

	/*
	파티클 contact들 만큼 contact 해결은 루틴을 돈다.
	하나의 reslover 인스턴스는 전체 시뮬레이션에서 공유될 수 있다.
	*/
	class ParticleContactResolver
	{
	protected:
		/*
		허가된 iteration의 숫자를 유지한다.
		*/
		unsigned iterations;

		/*
		이것은 traking value의 성능이다 - 우리는 사용되고 있는 iteration의 실제 숫자를 기록할 것이다.
		*/
		unsgiend iterationUsed;

	public:
		/*
		새로운 contact resolver를 생성한다.
		*/
		ParticleContactResolver(unsigned iterations);

		/*
		사용가능한 iteration의 수를 세팅한다.
		*/
		void setIterations(unsigned iterations);

		/*
		침투 및 속도에 대한 입자 접촉 세트를 해석한다.		
		*/
		void resolveContacts(ParticleContact* contactArray, unsigned numContacts, real duration);
	};

	/*
	이것은 파티클에 적용하는 접촉 생성기의 기본 다형 인터페이스이다.
	*/
	class ParticleContactGenerator
	{
	public:
		/*
		주어진 contact 구조를 생성된 contact로 채운다.
		contact pointer는 contact 배열의 첫 번째 사용 가능한 접점을
		가리켜야한다. 여기서 limit은 쓸 수 있는 배열의 최대 접점 수이다.
		메소드는 쓰여진 함수 수를 반환한다.
		*/
		virtual unsigned addContact(ParticleContact* contact, unsigned limit) const = 0;
	};

}