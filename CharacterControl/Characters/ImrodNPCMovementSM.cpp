#include "PrimeEngine/APIAbstraction/APIAbstractionDefines.h"

#include "PrimeEngine/Lua/LuaEnvironment.h"

#include "ImrodNPCMovementSM.h"
#include "ImrodNPC.h"
using namespace PE::Components;
using namespace PE::Events;
using namespace CharacterControl::Events;

namespace CharacterControl{

// Events sent by behavior state machine (or other high level state machines)
// these are events that specify where a imrod should move
namespace Events{

PE_IMPLEMENT_CLASS1(ImrodNPCMovementSM_Event_MOVE_TO, Event);

ImrodNPCMovementSM_Event_MOVE_TO::ImrodNPCMovementSM_Event_MOVE_TO(Vector3 targetPos /* = Vector3 */)
: m_targetPosition(targetPos)
, m_running(false)
{ }

PE_IMPLEMENT_CLASS1(ImrodNPCMovementSM_Event_STOP, Event);

PE_IMPLEMENT_CLASS1(ImrodNPCMovementSM_Event_TARGET_REACHED, Event);
}

namespace Components{

PE_IMPLEMENT_CLASS1(ImrodNPCMovementSM, Component);


ImrodNPCMovementSM::ImrodNPCMovementSM(PE::GameContext &context, PE::MemoryArena arena, PE::Handle hMyself) 
: Component(context, arena, hMyself)
, m_state(STANDING)
{}

SceneNode *ImrodNPCMovementSM::getParentsSceneNode()
{
	PE::Handle hParent = getFirstParentByType<Component>();
	if (hParent.isValid())
	{
		// see if parent has scene node component
		return hParent.getObject<Component>()->getFirstComponent<SceneNode>();
		
	}
	return NULL;
}

void ImrodNPCMovementSM::addDefaultComponents()
{
	Component::addDefaultComponents();

	PE_REGISTER_EVENT_HANDLER(ImrodNPCMovementSM_Event_MOVE_TO, ImrodNPCMovementSM::do_ImrodNPCMovementSM_Event_MOVE_TO);
	PE_REGISTER_EVENT_HANDLER(ImrodNPCMovementSM_Event_STOP, ImrodNPCMovementSM::do_ImrodNPCMovementSM_Event_STOP);
	
	PE_REGISTER_EVENT_HANDLER(Event_UPDATE, ImrodNPCMovementSM::do_UPDATE);
}

void ImrodNPCMovementSM::do_ImrodNPCMovementSM_Event_MOVE_TO(PE::Events::Event *pEvt)
{
	ImrodNPCMovementSM_Event_MOVE_TO *pRealEvt = (ImrodNPCMovementSM_Event_MOVE_TO *)(pEvt);
	
	// change state of this state machine
	
	m_targetPostion = pRealEvt->m_targetPosition;

	OutputDebugStringA("PE: PROGRESS: ImrodNPCMovementSM::do_ImrodNPCMovementSM_Event_MOVE_TO: Received event, running: ");
	OutputDebugStringA(pRealEvt->m_running ? "true\n":"false\n");

	if (pRealEvt->m_running) {
		m_state = RUNNING_TO_TARGET;
		// make sure the animations are playing

		
	}
	else {
		m_state = WALKING_TO_TARGET;
		// make sure the animations are playing

		
	}

	
}

void ImrodNPCMovementSM::do_ImrodNPCMovementSM_Event_STOP(PE::Events::Event *pEvt)
{
	
}

void ImrodNPCMovementSM::do_UPDATE(PE::Events::Event *pEvt)
{
	if (m_state == WALKING_TO_TARGET || m_state == RUNNING_TO_TARGET)
	{
		// see if parent has scene node component
		SceneNode *pSN = getParentsSceneNode();
		if (pSN)
		{
			Vector3 curPos = pSN->m_base.getPos();
			float dsqr = (m_targetPostion - curPos).lengthSqr();

			bool reached = true;
			if (dsqr > 0.01f)
			{
				// not at the spot yet
				Event_UPDATE *pRealEvt = (Event_UPDATE *)(pEvt);
				float speed = (m_state == WALKING_TO_TARGET)?1.4f:3.0f;

				//static float speed = 2.1f;
				float allowedDisp = speed * pRealEvt->m_frameTime;

				Vector3 dir = (m_targetPostion - curPos);
				dir.normalize();
				float dist = sqrt(dsqr);
				if (dist > allowedDisp)
				{
					dist = allowedDisp; // can move up to allowedDisp
					reached = false; // not reaching destination yet
				}

				// instantaneous turn
				pSN->m_base.turnInDirection(dir, 3.1415f);
				pSN->m_base.setPos(curPos + dir * dist);
			}

			if (reached)
			{
				m_state = STANDING;
				
				// target has been reached. need to notify all same level state machines (components of parent)
				{
					PE::Handle h("ImrodNPCMovementSM_Event_TARGET_REACHED", sizeof(ImrodNPCMovementSM_Event_TARGET_REACHED));
					Events::ImrodNPCMovementSM_Event_TARGET_REACHED *pOutEvt = new(h) ImrodNPCMovementSM_Event_TARGET_REACHED();

					PE::Handle hParent = getFirstParentByType<Component>();
					if (hParent.isValid())
					{
						hParent.getObject<Component>()->handleEvent(pOutEvt);
					}
					
					// release memory now that event is processed
					h.release();
				}

				if (m_state == STANDING)
				{
					// no one has modified our state based on TARGET_REACHED callback
					// this means we are not going anywhere right now
					// so can send event to animation state machine to stop
					{
						
					}
				}
			}
		}
	}
}

}}




