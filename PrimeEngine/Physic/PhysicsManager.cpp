#include "PrimeEngine/Scene/MeshInstance.h"
#include "PrimeEngine/Scene/MeshManager.h"
#include "PrimeEngine/Scene/RootSceneNode.h"
#include "PrimeEngine/Scene/Mesh.h"
#include "PrimeEngine/Scene/DebugRenderer.h"
#include "PrimeEngine/Lua/LuaEnvironment.h"

#include "PhysicsManager.h"
#include "PrimeEngine/Physic/Events/Events.h"

using namespace PE;
using namespace PE::Components;
using namespace PE::Events;

namespace PE {
namespace Components {

	PE_IMPLEMENT_CLASS1(PhysicsManager, Component);

	PhysicsManager::PhysicsManager(PE::GameContext &context, PE::MemoryArena arena, PE::Handle hMyself)
		:Component(context, arena, hMyself), physicsComponent(context,arena,100)
	{}

	void PhysicsManager::addDefaultComponents()
	{
		Component::addDefaultComponents();
		PE_REGISTER_EVENT_HANDLER(Event_UPDATE_PHYSIC, PhysicsManager::do_UPDATE_PHYSIC);
		PE_REGISTER_EVENT_HANDLER(Event_PRE_PHYSIC, PhysicsManager::do_PRE_PHYSIC);
	}

	void PhysicsManager::do_UPDATE_PHYSIC(PE::Events::Event *pEvt) {
		Event_UPDATE_PHYSIC *pRealEvent = (Event_UPDATE_PHYSIC*) pEvt;
		PhysicsManager::collisionTest(pRealEvent->m_frameTime);
	
	}

	void PhysicsManager::do_PRE_PHYSIC(PE::Events::Event *pEvt) {
		for(int i=0;i<physicsComponent.m_size; i++){
			PhysicsComponent *pPC = physicsComponent[i].getObject<PhysicsComponent>();
			pPC->do_PRE_UPDATE_PHYSIC();
		}
		

	}

	PrimitiveTypes::Bool PhysicsManager::boxSphereCollider(PhysicsComponent *box, PhysicsComponent *sphere) {

		Vector3 center = sphere->center;
		Vector3 bBmin = box->wbBmin;
		Vector3 bBmax = box->wbBmax;

		//if (isnan(center.m_x)|| isnan(center.m_y)|| isnan(center.m_z)) return false;
		
		
		PrimitiveTypes::Float32 m_x = max(bBmin.m_x,min(center.m_x,bBmax.m_x));
		PrimitiveTypes::Float32 m_y = max(bBmin.m_y, min(center.m_y, bBmax.m_y));
		PrimitiveTypes::Float32 m_z = max(bBmin.m_z, min(center.m_z, bBmax.m_z));

		PrimitiveTypes::Float32 distance = sqrt((m_x - center.m_x)*(m_x - center.m_x) + (m_y - center.m_y)*(m_y - center.m_y) + (m_z - center.m_z)*(m_z - center.m_z));

		return distance < sphere->m_sphereRadius;

	}

	PrimitiveTypes::Bool PhysicsManager::boxBoxCollider(PhysicsComponent *box, PhysicsComponent *otherbox) {

		Vector3 bBmin = box->bBmin;
		Vector3 bBmax = box->bBmax;
		Vector3 otherbBmin = otherbox->bBmin;
		Vector3 otherbBmax = otherbox->bBmax;

		return (bBmin.m_x <= otherbBmax.m_x && bBmax.m_x >= otherbBmin.m_x) && (bBmin.m_y <= otherbBmax.m_y && bBmax.m_y >= otherbBmin.m_y) && (bBmin.m_z <= otherbBmax.m_z && bBmax.m_z >= otherbBmin.m_z);
	}

	PrimitiveTypes::Bool PhysicsManager::sphereSphereCollider(PhysicsComponent *sphere, PhysicsComponent *othersphere) {

		Vector3 center = sphere->center;
		Vector3 othercenter = othersphere->center;

		PrimitiveTypes::Float32 distance = sqrt((center.m_x - othercenter.m_x)*(center.m_x - othercenter.m_x) + (center.m_y - othercenter.m_y)*(center.m_y - othercenter.m_y) + (center.m_z - othercenter.m_z)*(center.m_z - othercenter.m_z));
		return distance < (sphere->m_sphereRadius + othersphere->m_sphereRadius);
	}

	void PhysicsManager::collisionTest(PrimitiveTypes::Float32 frametime) {
	
		for (int i=0; i < physicsComponent.m_size; i++) {
			for (int j=0; j < physicsComponent.m_size; j++) {
				PhysicsComponent *pPC = physicsComponent[i].getObject<PhysicsComponent>();
				PhysicsComponent *otherpPC = physicsComponent[j].getObject<PhysicsComponent>();
				if (pPC->dynamicMesh) {
					pPC->do_GRAVITY_PHYSIC(frametime);
				}

				if (i == j) continue;

				
				if (pPC->m_physicPlane == PE::Components::PhysicsComponent::physicPlane::box && otherpPC->m_physicPlane == PE::Components::PhysicsComponent::physicPlane::box) {
					if (boxBoxCollider(pPC,otherpPC)) {

					}
				}
				if (pPC->m_physicPlane == PE::Components::PhysicsComponent::physicPlane::sphere && otherpPC->m_physicPlane == PE::Components::PhysicsComponent::physicPlane::sphere) {
					if (sphereSphereCollider(pPC, otherpPC)) {}
				}
				if (pPC->m_physicPlane == PE::Components::PhysicsComponent::physicPlane::box && otherpPC->m_physicPlane == PE::Components::PhysicsComponent::physicPlane::sphere) {
					if (boxSphereCollider(pPC, otherpPC)) {
						Vector3 d;
						PrimitiveTypes::Float32 dp;
						Vector3 u;
						Vector3 v;

						if (otherpPC->center.m_x > pPC->wbBmax.m_x) d.m_x = pPC->wbBmax.m_x;
						else if(otherpPC->center.m_x < pPC->wbBmin.m_x) d.m_x = pPC->wbBmin.m_x;
						else d.m_x = otherpPC->center.m_x;

						if (otherpPC->center.m_y > pPC->wbBmax.m_y) d.m_y = pPC->wbBmax.m_y;
						else if (otherpPC->center.m_y < pPC->wbBmin.m_y) d.m_y = pPC->wbBmin.m_y;
						else d.m_y = otherpPC->center.m_y;

						if (otherpPC->center.m_z > pPC->wbBmax.m_z) d.m_z = pPC->wbBmax.m_z;
						else if (otherpPC->center.m_z < pPC->wbBmin.m_z) d.m_z = pPC->wbBmin.m_z;
						else d.m_z = otherpPC->center.m_z;

						dp = otherpPC->m_sphereRadius - (otherpPC->center - d).length();
						u = (otherpPC->center - d);
						if(u.m_x != 0 && u.m_y != 0 && u.m_z != 0)
						u.normalize();

						SceneNode *pSN = otherpPC->getFirstParentByTypePtr<SceneNode>();
						v = pSN->m_base.getPos() + (u*dp);
						pSN->m_base.setPos(v);
					}
				}
			}
		}
	}



};
};