#include "PrimeEngine/Scene/MeshInstance.h"
#include "PrimeEngine/Scene/MeshManager.h"
#include "PrimeEngine/Scene/RootSceneNode.h"
#include "PrimeEngine/Scene/Mesh.h"
#include "PrimeEngine/Scene/DebugRenderer.h"
#include "PrimeEngine/Lua/LuaEnvironment.h"

#include "PhysicsComponent.h"
#include "PhysicsManager.h"

using namespace PE;
using namespace PE::Components;
using namespace PE::Events;

namespace PE {
namespace Components {

	PE_IMPLEMENT_CLASS1(PhysicsComponent, Component);

	PhysicsComponent::PhysicsComponent(PE::GameContext &context, PE::MemoryArena arena, PE::Handle hMyself)
		:Component(context, arena, hMyself)
	{}

	void PhysicsComponent::addDefaultComponents()
	{
		Component::addDefaultComponents();
		PE_REGISTER_EVENT_HANDLER(Event_PRE_RENDER_needsRC, PhysicsComponent::do_PRE_RENDER_needsRC);
	}
		
	void PhysicsComponent::do_PRE_RENDER_needsRC(PE::Events::Event *pEvt) {
		Mesh *pMesh = m_hAsset.getObject<Mesh>();

		if (!pMesh) return;

		SceneNode *pSN = getFirstParentByTypePtr<SceneNode>();

		if (m_physicPlane == box)
		{

			Vector3 fbl = pMesh->bBmin;
			Vector3 btr = pMesh->bBmax;
			Vector3 fbr(pMesh->bBmax.m_x, pMesh->bBmin.m_y, pMesh->bBmin.m_z);
			Vector3 ftl(pMesh->bBmin.m_x, pMesh->bBmax.m_y, pMesh->bBmin.m_z);
			Vector3 ftr(pMesh->bBmax.m_x, pMesh->bBmax.m_y, pMesh->bBmin.m_z);
			Vector3 bbl(pMesh->bBmin.m_x, pMesh->bBmin.m_y, pMesh->bBmax.m_z);
			Vector3 bbr(pMesh->bBmax.m_x, pMesh->bBmin.m_y, pMesh->bBmax.m_z);
			Vector3 btl(pMesh->bBmin.m_x, pMesh->bBmax.m_y, pMesh->bBmax.m_z);
			
			fbl = fbl + pSN->m_worldTransform.getPos();
			btr = btr + pSN->m_worldTransform.getPos();
			fbr = fbr + pSN->m_worldTransform.getPos();
			ftl = ftl + pSN->m_worldTransform.getPos();
			ftr = ftr + pSN->m_worldTransform.getPos();
			bbl = bbl + pSN->m_worldTransform.getPos();
			bbr = bbr + pSN->m_worldTransform.getPos();
			btl = btl + pSN->m_worldTransform.getPos();
			
			Vector3 color(0, 1.0f, 0);
			Vector3 linefbltofbr[] = { fbl, color, fbr, color };
			DebugRenderer::Instance()->createLineMesh(false, Matrix4x4(), &linefbltofbr[0].m_x, 2, 0);
			Vector3 linefbrtobbr[] = { fbr, color, bbr, color };
			DebugRenderer::Instance()->createLineMesh(false, Matrix4x4(), &linefbrtobbr[0].m_x, 2, 0);
			Vector3 linebbrtobbl[] = { bbr, color, bbl, color };
			DebugRenderer::Instance()->createLineMesh(false, Matrix4x4(), &linebbrtobbl[0].m_x, 2, 0);
			Vector3 linebbltofbl[] = { bbl, color, fbl, color };
			DebugRenderer::Instance()->createLineMesh(false, Matrix4x4(), &linebbltofbl[0].m_x, 2, 0);
			Vector3 linefbltoftl[] = { fbl, color, ftl, color };
			DebugRenderer::Instance()->createLineMesh(false, Matrix4x4(), &linefbltoftl[0].m_x, 2, 0);
			Vector3 linefbrtoftr[] = { fbr, color, ftr, color };
			DebugRenderer::Instance()->createLineMesh(false, Matrix4x4(), &linefbrtoftr[0].m_x, 2, 0);
			Vector3 linebbrtobtr[] = { bbr, color, btr, color };
			DebugRenderer::Instance()->createLineMesh(false, Matrix4x4(), &linebbrtobtr[0].m_x, 2, 0);
			Vector3 linebbltobtl[] = { bbl, color, btl, color };
			DebugRenderer::Instance()->createLineMesh(false, Matrix4x4(), &linebbltobtl[0].m_x, 2, 0);
			Vector3 lineftltoftr[] = { ftl, color, ftr, color };
			DebugRenderer::Instance()->createLineMesh(false, Matrix4x4(), &lineftltoftr[0].m_x, 2, 0);
			Vector3 lineftrtobtr[] = { ftr, color, btr, color };
			DebugRenderer::Instance()->createLineMesh(false, Matrix4x4(), &lineftrtobtr[0].m_x, 2, 0);
			Vector3 linebtrtobtl[] = { btr, color, btl, color };
			DebugRenderer::Instance()->createLineMesh(false, Matrix4x4(), &linebtrtobtl[0].m_x, 2, 0);
			Vector3 linebtltoftl[] = { btl, color, ftl, color };
			DebugRenderer::Instance()->createLineMesh(false, Matrix4x4(), &linebtltoftl[0].m_x, 2, 0);

		}
		else if (m_physicPlane == sphere) {

			Vector3 fbl = pMesh->bBmin;
			Vector3 btr = pMesh->bBmax;
						
			PrimitiveTypes::Float32 height = (btr.m_y - fbl.m_y) / 2;

			Vector3 meshPosition = pSN->m_worldTransform.getPos();
			meshPosition.m_y = meshPosition.m_y + height;

			Vector3 top = meshPosition;
			Vector3 bottom = meshPosition;
			Vector3 left = meshPosition;
			Vector3 right = meshPosition;
			Vector3 front = meshPosition;
			Vector3 back = meshPosition;

			top.m_y = top.m_y + m_sphereRadius;
			bottom.m_y = bottom.m_y - m_sphereRadius;
			left.m_x = left.m_x - m_sphereRadius;
			right.m_x = right.m_x + m_sphereRadius;
			front.m_z = front.m_z + m_sphereRadius;
			back.m_z = back.m_z - m_sphereRadius;

			Vector3 color(0, 1.0f, 0);
			Vector3 linetoptobottom[] = { top, color, bottom, color };
			DebugRenderer::Instance()->createLineMesh(false, Matrix4x4(), &linetoptobottom[0].m_x, 2, 0);
			Vector3 linelefttoright[] = { left, color, right, color };
			DebugRenderer::Instance()->createLineMesh(false, Matrix4x4(), &linelefttoright[0].m_x, 2, 0);
			Vector3 linefronttoback[] = { front, color, back, color };
			DebugRenderer::Instance()->createLineMesh(false, Matrix4x4(), &linefronttoback[0].m_x, 2, 0);
		}
	}

	void PhysicsComponent::do_GRAVITY_PHYSIC(PrimitiveTypes::Float32 frametime) {
		SceneNode *pSN = getFirstParentByTypePtr<SceneNode>();
		Vector3 newPos = pSN->m_base.getPos();
		newPos.m_y = newPos.m_y - (0.98f*frametime);
		pSN->m_base.setPos(newPos);
	
	}

	void PhysicsComponent::do_PRE_UPDATE_PHYSIC() {
		Mesh *pMesh = m_hAsset.getObject<Mesh>();		
				
		if (!pMesh) return;

		SceneNode *pSN = getFirstParentByTypePtr<SceneNode>();

		if(m_physicPlane == box)
		{
			bBmin = pMesh->bBmin;
			bBmax = pMesh->bBmax;

			center = pSN->m_base.getPos();

			Vector3 fbl = pMesh->bBmin;
			Vector3 btr = pMesh->bBmax;
			Vector3 fbr(pMesh->bBmax.m_x, pMesh->bBmin.m_y, pMesh->bBmin.m_z);
			Vector3 ftl(pMesh->bBmin.m_x, pMesh->bBmax.m_y, pMesh->bBmin.m_z);
			Vector3 ftr(pMesh->bBmax.m_x, pMesh->bBmax.m_y, pMesh->bBmin.m_z);
			Vector3 bbl(pMesh->bBmin.m_x, pMesh->bBmin.m_y, pMesh->bBmax.m_z);
			Vector3 bbr(pMesh->bBmax.m_x, pMesh->bBmin.m_y, pMesh->bBmax.m_z);
			Vector3 btl(pMesh->bBmin.m_x, pMesh->bBmax.m_y, pMesh->bBmax.m_z);

			m_fbl = fbl;
			m_btr = btr;
			m_fbr = fbr;
			m_ftl = ftl;
			m_ftr = ftr;
			m_bbl = bbl;
			m_bbr = bbr;
			m_btl = btl;

			m_fbl = m_fbl + pSN->m_worldTransform.getPos();
			m_btr = m_btr + pSN->m_worldTransform.getPos();
			m_fbr = m_fbr + pSN->m_worldTransform.getPos();
			m_ftl = m_ftl + pSN->m_worldTransform.getPos();
			m_ftr = m_ftr + pSN->m_worldTransform.getPos();
			m_bbl = m_bbl + pSN->m_worldTransform.getPos();
			m_bbr = m_bbr + pSN->m_worldTransform.getPos();
			m_btl = m_btl + pSN->m_worldTransform.getPos();

			wbBmin = bBmin + pSN->m_worldTransform.getPos();
			wbBmax = bBmax + pSN->m_worldTransform.getPos();


			/*Vector3 color(0, 1.0f, 0);
			Vector3 linefbltofbr[] = { m_fbl, color, m_fbr, color };
			DebugRenderer::Instance()->createLineMesh(false, Matrix4x4(), &linefbltofbr[0].m_x, 2, 0);
			Vector3 linefbrtobbr[] = { m_fbr, color, m_bbr, color };
			DebugRenderer::Instance()->createLineMesh(false, Matrix4x4(), &linefbrtobbr[0].m_x, 2, 0);
			Vector3 linebbrtobbl[] = { m_bbr, color, m_bbl, color };
			DebugRenderer::Instance()->createLineMesh(false, Matrix4x4(), &linebbrtobbl[0].m_x, 2, 0);
			Vector3 linebbltofbl[] = { m_bbl, color, m_fbl, color };
			DebugRenderer::Instance()->createLineMesh(false, Matrix4x4(), &linebbltofbl[0].m_x, 2, 0);
			Vector3 linefbltoftl[] = { m_fbl, color, m_ftl, color };
			DebugRenderer::Instance()->createLineMesh(false, Matrix4x4(), &linefbltoftl[0].m_x, 2, 0);
			Vector3 linefbrtoftr[] = { m_fbr, color, m_ftr, color };
			DebugRenderer::Instance()->createLineMesh(false, Matrix4x4(), &linefbrtoftr[0].m_x, 2, 0);
			Vector3 linebbrtobtr[] = { m_bbr, color, m_btr, color };
			DebugRenderer::Instance()->createLineMesh(false, Matrix4x4(), &linebbrtobtr[0].m_x, 2, 0);
			Vector3 linebbltobtl[] = { m_bbl, color, m_btl, color };
			DebugRenderer::Instance()->createLineMesh(false, Matrix4x4(), &linebbltobtl[0].m_x, 2, 0);
			Vector3 lineftltoftr[] = { m_ftl, color, m_ftr, color };
			DebugRenderer::Instance()->createLineMesh(false, Matrix4x4(), &lineftltoftr[0].m_x, 2, 0);
			Vector3 lineftrtobtr[] = { m_ftr, color, m_btr, color };
			DebugRenderer::Instance()->createLineMesh(false, Matrix4x4(), &lineftrtobtr[0].m_x, 2, 0);
			Vector3 linebtrtobtl[] = { m_btr, color, m_btl, color };
			DebugRenderer::Instance()->createLineMesh(false, Matrix4x4(), &linebtrtobtl[0].m_x, 2, 0);
			Vector3 linebtltoftl[] = { m_btl, color, m_ftl, color };
			DebugRenderer::Instance()->createLineMesh(false, Matrix4x4(), &linebtltoftl[0].m_x, 2, 0);*/
		
		}
		else if (m_physicPlane == sphere) {

			Vector3 fbl = pMesh->bBmin;
			Vector3 btr = pMesh->bBmax;

			center = pSN->m_worldTransform.getPos();
			

			PrimitiveTypes::Float32 height = (btr.m_y - fbl.m_y)/2;

			Vector3 meshPosition = pSN->m_worldTransform.getPos();
			meshPosition.m_y = meshPosition.m_y + height;
			center.m_y = center.m_y + height;

			Vector3 top = meshPosition;
			Vector3 bottom = meshPosition;
			Vector3 left = meshPosition;
			Vector3 right = meshPosition;
			Vector3 front = meshPosition;
			Vector3 back = meshPosition;

			top.m_y = top.m_y + m_sphereRadius;
			bottom.m_y = bottom.m_y - m_sphereRadius;
			left.m_x = left.m_x - m_sphereRadius;
			right.m_x = right.m_x + m_sphereRadius;
			front.m_z = front.m_z + m_sphereRadius;
			back.m_z = back.m_z - m_sphereRadius;

			/*Vector3 color(0, 1.0f, 0);
			Vector3 linetoptobottom[] = { top, color, bottom, color };
			DebugRenderer::Instance()->createLineMesh(false, Matrix4x4(), &linetoptobottom[0].m_x, 2, 0);
			Vector3 linelefttoright[] = { left, color, right, color };
			DebugRenderer::Instance()->createLineMesh(false, Matrix4x4(), &linelefttoright[0].m_x, 2, 0);
			Vector3 linefronttoback[] = { front, color, back, color };
			DebugRenderer::Instance()->createLineMesh(false, Matrix4x4(), &linefronttoback[0].m_x, 2, 0);*/
		}
		
	}

};
};