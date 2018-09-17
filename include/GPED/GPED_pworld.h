#ifndef __GPED_PWORLD_H__
#define __GPED_PWORLD_H__

#include <vector>

#include "GPED_Pcontact.h"
#include "GPED_Pfgen.h"

namespace GPED
{
	/*
	파티클 세트를 추적하고 모든 파티클을 업데이트 할 수 있는 방법을 제공한다
	*/
	class ParticleWorld
	{
	public:
		typedef std::vector<GPEDParticle*> Particles;
		typedef std::vector<ParticleContactGenerator*> ContactGenerators;
	
	protected:
		/*
		파티클들을 유지한다
		*/
		Particles particles;

		/*
		세계가 각 프레임에서 컨택 결정자에게 주는 반복 횟수를
		계산해야하는 경우에 true이다
		*/
		bool calculaterations;

		/*
		파티클의 힘 발생기를 유지한다
		*/
		ParticleForceRegistry registry;

		/*
		contact에 대한 resolver를 유지한다
		*/
		ParticleForceRegistry resolver;

		/*
		contact 발생기
		*/
		ContactGenerators contactGenerators;

		/*
		contact의 목록을 유지한다
		*/
		ParticleContact* contacts;

		/*
		허용되는 최대 contact 수를 나타낸다 (즉, contact 배열의 크기)
		*/
		unsigned maxContacts;
	
	public:
		/*
		프레임 당 주어진 개수의 접촉을 처리할 수 있는 새로운 파티클 시뮬레이터를 생성한다
		또한, 선택적으로 사용할 contact 확인 반복 횟수를 지정할 수 있다
		반복 횟수를 지정하지 않으면 contact 수가 두 번 사용된다
		*/
		ParticleWorld(unsigned maxContacts, unsigned iterations = 0);
		~ParticleWorld();

		/*
		등록된 각 contact 생성자에게 연락해 해당 contact를 보고한다
		생성된 contact 수를 반환한다
		*/
		unsigned generateContacts();


		/*
		이 세계의 모든 파티클을 주어진 시간만큼 앞으로 진행한다
		*/
		void runPhysics(real duration);

		/*
		시뮬레이션 프레임을 위한 세계를 초기화한다
		이것은 세계의 파티클에 대한 누적된 힘을 제거한다
		이것을 호출 한 후에 파티클은 이 프레임에 대한 힘을 가질 수 있다
		*/
		void startFrame();


		/*
		파티클의 목록을 리턴한다
		*/
		Particles& getParticles();

		/*
		contact 생성기 목록을 리턴한다
		*/
		ContactGenerators& getContactGenerators();

		/*
		force 레지스트리를 돌려준다
		*/
		ParticleForceRegistry& getForceRegistry();
	};

	/*
	파티클 포인터의 STL 벡터를 가져와서 지면과 충돌시키는 접촉 생성기이다
	*/
	class GroundContacts : public ParticleContactGenerator
	{
		ParticleWorld::Particles* particles;

	public:
		void init(ParticleWorld::Particles* particles);

		virtual unsigned addContact(ParticleContact* contact, unsigned limit) const;
	};

}

#endif