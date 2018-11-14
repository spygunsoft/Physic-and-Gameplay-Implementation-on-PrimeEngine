#ifndef __pe_meshinstance_h__
#define __pe_meshinstance_h__

#define NOMINMAX
// API Abstraction
#include "PrimeEngine/APIAbstraction/APIAbstractionDefines.h"

// Outer-Engine includes
#include <assert.h>

// Inter-Engine includes
#include "PrimeEngine/APIAbstraction/Effect/Effect.h"

// Sibling/Children includes
#include "Mesh.h"

namespace PE {
namespace Components {

struct MeshInstance : public Component
{
	PE_DECLARE_CLASS(MeshInstance);

	PE_DECLARE_IMPLEMENT_EVENT_HANDLER_WRAPPER(do_PRE_RENDER_needsRC)
	void do_PRE_RENDER_needsRC(PE::Events::Event *pEvt);

	// Constructor -------------------------------------------------------------
	MeshInstance(PE::GameContext &context, PE::MemoryArena arena, Handle hMyself) ;

	void initFromFile(const char *assetName, const char *assetPackage,
		int &threadOwnershipMask);

	void initFromRegisteredAsset(const PE::Handle &h);

	virtual ~MeshInstance(){}

	virtual void addDefaultComponents();

	bool hasSkinWeights();

    bool m_culledOut;
	Handle m_hAsset;

	Vector3 m_fbl;
	Vector3 m_btr;
	Vector3 m_fbr;
	Vector3 m_ftl;
	Vector3 m_ftr;
	Vector3 m_bbl;
	Vector3 m_bbr;
	Vector3 m_btl;

	int m_skinDebugVertexId;

};

}; // namespace Components
}; // namespace PE
#endif
