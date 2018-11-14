#ifndef _PHYSICS_MANAGER_
#define _PHYSICS_MANAGER_

#include "../Events/Component.h"
#include "PhysicsComponent.h"

namespace PE {
namespace Components {

	struct PhysicsManager : public PE::Components::Component {

		PE_DECLARE_CLASS(PhysicsManager);

		PE_DECLARE_IMPLEMENT_EVENT_HANDLER_WRAPPER(do_UPDATE_PHYSIC);		
		void do_UPDATE_PHYSIC(PE::Events::Event *pEvt);

		PE_DECLARE_IMPLEMENT_EVENT_HANDLER_WRAPPER(do_PRE_PHYSIC);
		void do_PRE_PHYSIC(PE::Events::Event *pEvt);

		PhysicsManager(PE::GameContext &context, PE::MemoryArena arena, PE::Handle hMyself);

		virtual void addDefaultComponents();

		void collisionTest(PrimitiveTypes::Float32 frametime);

		PrimitiveTypes::Bool boxSphereCollider(PhysicsComponent *box, PhysicsComponent *sphere);

		PrimitiveTypes::Bool boxBoxCollider(PhysicsComponent *box, PhysicsComponent *otherbox);

		PrimitiveTypes::Bool sphereSphereCollider(PhysicsComponent *sphere, PhysicsComponent *othersphere);

		Array<Handle,1> physicsComponent;

		Handle m_hAsset;


	};
	
};
};


#endif