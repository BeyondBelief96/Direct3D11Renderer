#include "Renderable/Model/Mesh.h"
#include "Bindable/BindableCommon.h"

void Mesh::Submit(FrameManager& frameManager, DirectX::FXMMATRIX accumulatedTransform) const noexcept
{
    DirectX::XMStoreFloat4x4(&transform, accumulatedTransform);
	this->Renderable::Submit(frameManager);
}

DirectX::XMMATRIX Mesh::GetTransformXM() const noexcept
{
    return DirectX::XMLoadFloat4x4(&transform);
}


