#include "Renderable/Model/Mesh.h"
#include "Bindable/BindableCommon.h"

Mesh::Mesh(Graphics& gfx, std::vector<std::unique_ptr<Bindable>> inBindables)
{
    // Register all bindables to the graphics pipeline to draw this mesh.
    for (auto& b : inBindables)
    {
        AddUniqueBindable(std::move(b));
    }

    // Add per-mesh transform constant buffer
    AddUniqueBindable(std::make_unique<TransformConstantBuffer>(gfx, *this));
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


