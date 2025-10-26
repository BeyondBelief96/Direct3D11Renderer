#pragma once

#include "Renderable/Renderable.h"
#include "Core/Graphics.h"
#include "Bindable/Bindable.h"
#include <DirectXMath.h>
#include <memory>
#include <vector>
#include <string>

namespace D3
{
	class Material;
}

class Mesh : public Renderable
{
public:
    Mesh(Graphics& gfx, const D3::Material& material, const aiMesh& mesh) noexcept;
	void Submit(FrameManager& frameManager, DirectX::FXMMATRIX accumulatedTransform) const noexcept;
    DirectX::XMMATRIX GetTransformXM() const noexcept override;

private:
    mutable DirectX::XMFLOAT4X4 transform{};
};


