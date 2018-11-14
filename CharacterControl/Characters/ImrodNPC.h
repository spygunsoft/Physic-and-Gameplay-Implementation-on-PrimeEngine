#ifndef _CHARACTER_CONTROL_IMROD_NPC_
#define _CHARACTER_CONTROL_IMROD_NPC_

#include "PrimeEngine/Events/Component.h"


#include "../Events/Events.h"

namespace CharacterControl {

	namespace Components {

		struct ImrodNPC : public PE::Components::Component
		{
			PE_DECLARE_CLASS(ImrodNPC);

			ImrodNPC(PE::GameContext &context, PE::MemoryArena arena, PE::Handle hMyself, Events::Event_CreateImrodNPC *pEvt);

			virtual void addDefaultComponents();

			char m_targetName[32];
			Matrix4x4 m_base;
		};
	}; // namespace Components
}; // namespace CharacterControl
#endif

