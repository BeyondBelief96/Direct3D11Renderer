#pragma once

#include "Renderable/Renderable.h"
#include "Core/Graphics.h"
#include "Bindable/Bindable.h"
#include <DirectXMath.h>
#include <memory>
#include <vector>

class Mesh : public Renderable
{
public:
	void Submit(FrameManager& frameManager, DirectX::FXMMATRIX accumulatedTransform) const noexcept;
    DirectX::XMMATRIX GetTransformXM() const noexcept override;

private:
    mutable DirectX::XMFLOAT4X4 transform{};
};


