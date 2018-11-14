#include "PrimeEngine/APIAbstraction/APIAbstractionDefines.h"

#include "PrimeEngine/Lua/LuaEnvironment.h"
#include "PrimeEngine/Scene/DebugRenderer.h"
#include "../ClientGameObjectManagerAddon.h"
#include "../CharacterControlContext.h"
#include "ImrodNPCMovementSM.h"
#include "ImrodNPCBehaviorSM.h"
#include "ImrodNPC.h"
#include "PrimeEngine/Scene/SceneNode.h"
#include "PrimeEngine/Render/IRenderer.h"
using namespace PE::Components;
using namespace PE::Events;
using namespace CharacterControl::Events;

namespace CharacterControl {

	namespace Components {

		PE_IMPLEMENT_CLASS1(ImrodNPCBehaviorSM, Component);

		ImrodNPCBehaviorSM::ImrodNPCBehaviorSM(PE::GameContext &context, PE::MemoryArena arena, PE::Handle hMyself, PE::Handle hMovementSM)
			: Component(context, arena, hMyself)
			, m_hMovementSM(hMovementSM)
		{

		}

		void ImrodNPCBehaviorSM::start()
		{
			if (m_havePatrolWayPoint)
			{
				m_state = WAITING_FOR_WAYPOINT; // will update on next do_UPDATE()
			}
			else
			{
				m_state = IDLE; // stand in place

				PE::Handle h("ImrodNPCMovementSM_Event_STOP", sizeof(ImrodNPCMovementSM_Event_STOP));
				ImrodNPCMovementSM_Event_STOP *pEvt = new(h) ImrodNPCMovementSM_Event_STOP();

				m_hMovementSM.getObject<Component>()->handleEvent(pEvt);
				// release memory now that event is processed
				h.release();

			}
		}

		void ImrodNPCBehaviorSM::addDefaultComponents()
		{
			Component::addDefaultComponents();

			PE_REGISTER_EVENT_HANDLER(ImrodNPCMovementSM_Event_TARGET_REACHED, ImrodNPCBehaviorSM::do_ImrodNPCMovementSM_Event_TARGET_REACHED);
			PE_REGISTER_EVENT_HANDLER(Event_UPDATE, ImrodNPCBehaviorSM::do_UPDATE);

			PE_REGISTER_EVENT_HANDLER(Event_PRE_RENDER_needsRC, ImrodNPCBehaviorSM::do_PRE_RENDER_needsRC);
		}

		//sent by movement state machine whenever it reaches current target
		void ImrodNPCBehaviorSM::do_ImrodNPCMovementSM_Event_TARGET_REACHED(PE::Events::Event *pEvt)
		{
			PEINFO("ImrodNPCBehaviorSM::do_ImrodNPCMovementSM_Event_TARGET_REACHED\n");

			if (m_state == PATROLLING_WAYPOINTS)
			{
				ClientGameObjectManagerAddon *pGameObjectManagerAddon = (ClientGameObjectManagerAddon *)(m_pContext->get<CharacterControlContext>()->getGameObjectManagerAddon());
				if (pGameObjectManagerAddon)
				{
					// search for waypoint object
					WayPoint *pWP = pGameObjectManagerAddon->getWayPoint(m_curPatrolWayPoint);
					if (pWP && StringOps::length(pWP->m_nextWayPointName) > 0)
					{
						// have next waypoint to go to. Random from Waypoint List
						pWP = pGameObjectManagerAddon->getWayPoint(pWP->m_nextWaypointList[rand() % pWP->m_count]);
						if (pWP)
						{
							StringOps::writeToString(pWP->m_name, m_curPatrolWayPoint, 32);

							m_state = PATROLLING_WAYPOINTS;
							PE::Handle h("ImrodNPCMovementSM_Event_MOVE_TO", sizeof(ImrodNPCMovementSM_Event_MOVE_TO));
							Events::ImrodNPCMovementSM_Event_MOVE_TO *pEvt = new(h) ImrodNPCMovementSM_Event_MOVE_TO(pWP->m_base.getPos());



							pEvt->m_running = pWP->m_needToRunToThisWaypoint > 0;

							m_hMovementSM.getObject<Component>()->handleEvent(pEvt);
							// release memory now that event is processed
							h.release();
						}
					}
					else
					{
						m_state = IDLE;
						// no need to send the event. movement state machine will automatically send event to animation state machine to play idle animation
					}
				}
			}
		}

		// this event is executed when thread has RC
		void ImrodNPCBehaviorSM::do_PRE_RENDER_needsRC(PE::Events::Event *pEvt)
		{
			Event_PRE_RENDER_needsRC *pRealEvent = (Event_PRE_RENDER_needsRC *)(pEvt);
			if (m_havePatrolWayPoint)
			{
				char buf[80];
				sprintf(buf, "Patrol Waypoint: %s", m_curPatrolWayPoint);
				ImrodNPC *pSol = getFirstParentByTypePtr<ImrodNPC>();
				PE::Handle hImrodSceneNode = pSol->getFirstComponentHandle<PE::Components::SceneNode>();
				Matrix4x4 base = hImrodSceneNode.getObject<PE::Components::SceneNode>()->m_worldTransform;

				DebugRenderer::Instance()->createTextMesh(
					buf, false, false, true, false, 0,
					base.getPos(), 0.01f, pRealEvent->m_threadOwnershipMask);

				{
					//we can also construct points ourself
					bool sent = false;
					ClientGameObjectManagerAddon *pGameObjectManagerAddon = (ClientGameObjectManagerAddon *)(m_pContext->get<CharacterControlContext>()->getGameObjectManagerAddon());
					if (pGameObjectManagerAddon)
					{
						WayPoint *pWP = pGameObjectManagerAddon->getWayPoint(m_curPatrolWayPoint);
						if (pWP)
						{
							Vector3 target = pWP->m_base.getPos();
							Vector3 pos = base.getPos();
							Vector3 color(1.0f, 1.0f, 0);
							Vector3 linepts[] = { pos, color, target, color };

							DebugRenderer::Instance()->createLineMesh(true, base, &linepts[0].m_x, 2, 0);// send event while the array is on the stack
							sent = true;
						}
					}
					if (!sent) // if for whatever reason we didnt retrieve waypoint info, send the event with transform only
						DebugRenderer::Instance()->createLineMesh(true, base, NULL, 0, 0);// send event while the array is on the stack
				}
			}
		}

		void ImrodNPCBehaviorSM::do_UPDATE(PE::Events::Event *pEvt)
		{
			if (m_state == WAITING_FOR_WAYPOINT)
			{
				if (m_havePatrolWayPoint)
				{
					ClientGameObjectManagerAddon *pGameObjectManagerAddon = (ClientGameObjectManagerAddon *)(m_pContext->get<CharacterControlContext>()->getGameObjectManagerAddon());
					if (pGameObjectManagerAddon)
					{
						// search for waypoint object
						WayPoint *pWP = pGameObjectManagerAddon->getWayPoint(m_curPatrolWayPoint);


						if (pWP)
						{
							m_state = PATROLLING_WAYPOINTS;
							PE::Handle h("ImrodNPCMovementSM_Event_MOVE_TO", sizeof(ImrodNPCMovementSM_Event_MOVE_TO));
							Events::ImrodNPCMovementSM_Event_MOVE_TO *pEvt = new(h) ImrodNPCMovementSM_Event_MOVE_TO(pWP->m_base.getPos());

							m_hMovementSM.getObject<Component>()->handleEvent(pEvt);
							// release memory now that event is processed
							h.release();
						}
					}
				}
				else
				{
					// should not happen, but in any case, set state to idle
					m_state = IDLE;

					PE::Handle h("ImrodNPCMovementSM_Event_STOP", sizeof(ImrodNPCMovementSM_Event_STOP));
					ImrodNPCMovementSM_Event_STOP *pEvt = new(h) ImrodNPCMovementSM_Event_STOP();

					m_hMovementSM.getObject<Component>()->handleEvent(pEvt);
					// release memory now that event is processed
					h.release();
				}
			}
		}


	}
}




