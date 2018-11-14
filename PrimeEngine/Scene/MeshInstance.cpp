#define NOMINMAX
// API Abstraction
#include "PrimeEngine/APIAbstraction/APIAbstractionDefines.h"

// Outer-Engine includes

// Inter-Engine includes
#include "PrimeEngine/FileSystem/FileReader.h"
#include "PrimeEngine/APIAbstraction/GPUMaterial/GPUMaterialSet.h"
#include "PrimeEngine/PrimitiveTypes/PrimitiveTypes.h"
#include "PrimeEngine/APIAbstraction/Texture/Texture.h"
#include "PrimeEngine/APIAbstraction/Effect/EffectManager.h"
#include "PrimeEngine/APIAbstraction/GPUBuffers/VertexBufferGPUManager.h"
#include "PrimeEngine/Scene/Skeleton.h"
#include "DefaultAnimationSM.h"
#include "Light.h"

#include "PrimeEngine/GameObjectModel/Camera.h"

// Sibling/Children includes
#include "MeshInstance.h"
#include "MeshManager.h"
#include "SceneNode.h"
#include "CameraManager.h"
#include "PrimeEngine/Lua/LuaEnvironment.h"
#include "PrimeEngine/Events/StandardEvents.h"
#include "PrimeEngine/Scene/DebugRenderer.h"

using namespace PE::Events;

namespace PE {
namespace Components{

PE_IMPLEMENT_CLASS1(MeshInstance, Component);

MeshInstance::MeshInstance(PE::GameContext &context, PE::MemoryArena arena, Handle hMyself)
: Component(context, arena, hMyself)
, m_culledOut(false)
{
	
}

void MeshInstance::addDefaultComponents()
{
	Component::addDefaultComponents();
	PE_REGISTER_EVENT_HANDLER(Event_PRE_RENDER_needsRC, MeshInstance::do_PRE_RENDER_needsRC);
}

void MeshInstance::initFromFile(const char *assetName, const char *assetPackage,
		int &threadOwnershipMask)
{
	Handle h = m_pContext->getMeshManager()->getAsset(assetName, assetPackage, threadOwnershipMask);

	initFromRegisteredAsset(h);
}

bool MeshInstance::hasSkinWeights()
{
	Mesh *pMesh = m_hAsset.getObject<Mesh>();
	return pMesh->m_hSkinWeightsCPU.isValid();
}

void MeshInstance::initFromRegisteredAsset(const PE::Handle &h)
{
	m_hAsset = h;
	//add this instance as child to Mesh so that skin knows what to draw
	static int allowedEvts[] = {0};
	m_hAsset.getObject<Component>()->addComponent(m_hMyself, &allowedEvts[0]);
}

void MeshInstance::do_PRE_RENDER_needsRC(PE::Events::Event *pEvt) 
{
	Mesh *pMesh = m_hAsset.getObject<Mesh>();

	if (!pMesh) return;

	SceneNode *pSN = getFirstParentByTypePtr<SceneNode>();

	if (!pSN) return;

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
	
	if (m_culledOut)  return;
	/*Vector3 color(1.0f, 0, 0);
	Vector3 linefbltofbr[] = { m_fbl, color, m_fbr, color };
	DebugRenderer::Instance()->createLineMesh(false, Matrix4x4(), &linefbltofbr[0].m_x,2, 0);
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

}; // namespace Components
}; // namespace PE
