#include "PrimeEngine/APIAbstraction/APIAbstractionDefines.h"

#include "PrimeEngine/Lua/LuaEnvironment.h"
#include "PrimeEngine/Scene/SkeletonInstance.h"
#include "PrimeEngine/Scene/MeshInstance.h"
#include "PrimeEngine/Scene/RootSceneNode.h"

#include "ImrodNPC.h""
#include "ImrodNPCMovementSM.h"
#include "ImrodNPCBehaviorSM.h"


using namespace PE;
using namespace PE::Components;
using namespace CharacterControl::Events;

namespace CharacterControl {
	namespace Components {

		PE_IMPLEMENT_CLASS1(ImrodNPC, Component);

		ImrodNPC::ImrodNPC(PE::GameContext &context, PE::MemoryArena arena, PE::Handle hMyself, Event_CreateImrodNPC *pEvt) : Component(context, arena, hMyself)
		{

			// hierarchy of imrod and replated components and variables (note variables are just variables, they are not passed events to)
			// scene
			// +-components
			//   +-imrod scene node
			//   | +-components
			//   |   +-imrod skin
			//   |     +-components
			//   |       +-imrod animation state machine
			//   |       +-imrod weapon skin scene node
			//   |         +-components
			//   |           +-weapon mesh

			// game objects
			// +-components
			//   +-imrod npc
			//     +-variables
			//     | +-m_hMySN = imrod scene node
			//     | +-m_hMySkin = skin
			//     | +-m_hMyGunSN = imrod weapon skin scene node
			//     | +-m_hMyGunMesh = weapon mesh
			//     +-components
			//       +-imrod scene node (restricted to no events. this is for state machines to be able to locate the scene node)
			//       +-movement state machine
			//       +-behavior state machine


			// need to acquire redner context for this code to execute thread-safe
			m_pContext->getGPUScreen()->AcquireRenderContextOwnership(pEvt->m_threadOwnershipMask);

			PE::Handle hSN("SCENE_NODE", sizeof(SceneNode));
			SceneNode *pMainSN = new(hSN) SceneNode(*m_pContext, m_arena, hSN);
			pMainSN->addDefaultComponents();

			pMainSN->m_base.setPos(pEvt->m_pos);
			pMainSN->m_base.setU(pEvt->m_u);
			pMainSN->m_base.setV(pEvt->m_v);
			pMainSN->m_base.setN(pEvt->m_n);

			StringOps::writeToString(pEvt->m_targetName,m_targetName,32);

			m_base = pMainSN->m_base;

			RootSceneNode::Instance()->addComponent(hSN);

			// add the scene node as component of imrod without any handlers. this is just data driven way to locate scnenode for Imrod's components
			{
				static int allowedEvts[] = { 0 };
				addComponent(hSN, &allowedEvts[0]);
			}

			PE::Handle hMeshInstance("MeshInstance", sizeof(MeshInstance));
			MeshInstance *pMeshInstance = new(hMeshInstance) MeshInstance(*m_pContext, m_arena, hMeshInstance);
			pMeshInstance->addDefaultComponents();

			pMeshInstance->initFromFile(pEvt->m_meshFilename, pEvt->m_package, pEvt->m_threadOwnershipMask);

			pMainSN->addComponent(hMeshInstance);

			m_pContext->getGPUScreen()->ReleaseRenderContextOwnership(pEvt->m_threadOwnershipMask);

#if 1
			// add movement state machine to imrod npc
			PE::Handle hImrodMovementSM("ImrodNPCMovementSM", sizeof(ImrodNPCMovementSM));
			ImrodNPCMovementSM *pImrodMovementSM = new(hImrodMovementSM) ImrodNPCMovementSM(*m_pContext, m_arena, hImrodMovementSM);
			pImrodMovementSM->addDefaultComponents();

			// add it to Imrod NPC
			addComponent(hImrodMovementSM);

			// add behavior state machine ot Imrod npc
			PE::Handle hImrodBheaviorSM("ImrodNPCBehaviorSM", sizeof(ImrodNPCBehaviorSM));
			ImrodNPCBehaviorSM *pImrodBehaviorSM = new(hImrodBheaviorSM) ImrodNPCBehaviorSM(*m_pContext, m_arena, hImrodBheaviorSM, hImrodMovementSM);
			pImrodBehaviorSM->addDefaultComponents();

			// add it to Imrod NPC
			addComponent(hImrodBheaviorSM);

			StringOps::writeToString(pEvt->m_patrolWayPoint, pImrodBehaviorSM->m_curPatrolWayPoint, 32);
			pImrodBehaviorSM->m_havePatrolWayPoint = StringOps::length(pImrodBehaviorSM->m_curPatrolWayPoint) > 0;

			// start the Imrod
			pImrodBehaviorSM->start();
#endif
		}

		void ImrodNPC::addDefaultComponents()
		{
			Component::addDefaultComponents();

			// custom methods of this component
		}

	}; // namespace Components
}; // namespace CharacterControl
