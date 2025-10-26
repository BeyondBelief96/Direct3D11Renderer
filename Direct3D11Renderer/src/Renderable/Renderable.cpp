#include "Renderable/Renderable.h"
#include "Bindable/BindableCommon.h"
#include "Bindable/BindableCache.h"
#include "Exceptions/GraphicsExceptions.h"
#include "Renderable/Material/Material.h"
#include <cassert>
#include <typeinfo>
#include <scene.h>

Renderable::Renderable(Graphics& gfx, const D3::Material& material, const aiMesh& mesh) noexcept
{
	pVertices = material.MakeVertexBufferBindable(gfx, mesh);
	pIndices = material.MakeIndexBufferBindable(gfx, mesh);
	pTopology = Topology::Resolve(gfx, D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	for (auto& technique : material.GetTechniques())
	{
		AddTechnique(std::move(technique));
	}
}

void Renderable::Submit(FrameManager& frameManager) const noexcept
{
	for (const auto& technique : techniques)
	{
		technique.Submit(frameManager, *this);
	}
}

void Renderable::AddTechnique(Technique technique) noexcept
{
	technique.InitializeParentReferences(*this);
	techniques.push_back(std::move(technique));
}

void Renderable::Bind(Graphics& gfx) const noexcept
{
	pVertices->Bind(gfx);
	pIndices->Bind(gfx);
	pTopology->Bind(gfx);
}

void Renderable::Accept(TechniqueProbe& probe)
{
	for (auto& technique : techniques)
	{
		technique.Accept(probe);
	}
}

UINT Renderable::GetIndexCount() const noexcept
{
	return pIndices->GetCount();
}