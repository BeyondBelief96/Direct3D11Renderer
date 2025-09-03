#include "Renderable/Model/Mesh.h"
#include "Bindable/BindableCommon.h"

Mesh::Mesh(Graphics& gfx, std::vector<std::shared_ptr<Bindable>> inBindables)
{
    // Register all bindables to the graphics pipeline to draw this mesh.
    for (auto& b : inBindables)
    {
        AddBindable(std::shared_ptr<Bindable>(std::move(b)));
    }

    // Add per-mesh transform constant buffer
    AddBindable(std::make_shared<TransformConstantBuffer>(gfx, *this));
}

void Mesh::Draw(Graphics& gfx, DirectX::FXMMATRIX accumulatedTransform) const noexcept
{
    DirectX::XMStoreFloat4x4(&transform, accumulatedTransform);
    Render(gfx);
}

DirectX::XMMATRIX Mesh::GetTransformXM() const noexcept
{
    return DirectX::XMLoadFloat4x4(&transform);
}


