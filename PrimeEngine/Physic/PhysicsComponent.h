#ifndef _PHYSICS_COMPONENT_
#define _PHYSICS_COMPONENT_

#include "../Events/Component.h"

//#include "PrimeEngine/Physic/Events/Events.h"

namespace PE {

namespace Components {

	struct PhysicsComponent : public PE::Components::Component {
		PE_DECLARE_CLASS(PhysicsComponent);

		PE_DECLARE_IMPLEMENT_EVENT_HANDLER_WRAPPER(do_PRE_RENDER_needsRC);

		enum physicPlane {sphere,box};
		
		void do_PRE_RENDER_needsRC(PE::Events::Event *pEvt);

		void do_PRE_UPDATE_PHYSIC();

		void do_GRAVITY_PHYSIC(PrimitiveTypes::Float32 frametime);

		PhysicsComponent(PE::GameContext &context, PE::MemoryArena arena, PE::Handle hMyself);


		virtual void addDefaultComponents();

		PrimitiveTypes::Bool m_culledOut;
		PrimitiveTypes::Float32 m_sphereRadius;

		physicPlane m_physicPlane;

		bool dynamicMesh = false;
		Handle m_hAsset;
		Vector3 bBmin;
		Vector3 bBmax;

		Vector3 wbBmin;
		Vector3 wbBmax;

		Vector3 center;

		Vector3 m_fbl;
		Vector3 m_btr;
		Vector3 m_fbr;
		Vector3 m_ftl;
		Vector3 m_ftr;
		Vector3 m_bbl;
		Vector3 m_bbr;
		Vector3 m_btl;
	};

};
};
#endif