#include "Renderable/Model/Mesh.h"
#include "Bindable/BindableCommon.h"

Mesh::Mesh(Graphics& gfx, const D3::Material& material, const aiMesh& mesh) noexcept
    : Renderable(gfx, material, mesh)
{
}

void Mesh::Submit(FrameManager& frameManager, DirectX::FXMMATRIX accumulatedTransform) const noexcept
{
    DirectX::XMStoreFloat4x4(&transform, accumulatedTransform);
	this->Renderable::Submit(frameManager);
}

DirectX::XMMATRIX Mesh::GetTransformXM() const noexcept
{
    return DirectX::XMLoadFloat4x4(&transform);
}


