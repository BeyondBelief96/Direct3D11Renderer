#pragma once

#include "Renderable/Renderable.h"
#include "Graphics/Graphics.h"
#include "Bindable/Bindable.h"
#include <DirectXMath.h>
#include <memory>
#include <vector>

/// <summary>
/// A mesh represents a single renderable object that the graphics pipeline will draw in the scene. A mesh can be re-used across 
/// many nodes throughout the scene graph. The mesh takes in all of the graphics pipeline bindables it needs to render.
/// </summary>
class Mesh : public Renderable
{
public:
    // Construct mesh with a prepared set of bindables (VB, IB, shaders, layout, constants, etc.)
    Mesh(Graphics& gfx, std::vector<std::unique_ptr<Bindable>> bindables);

    // Draw with an externally-supplied transform (typically from a scene graph node)
    void Draw(Graphics& gfx, DirectX::FXMMATRIX accumulatedTransform) const noexcept;

    DirectX::XMMATRIX GetTransformXM() const noexcept override;

private:
    mutable DirectX::XMFLOAT4X4 transform{};
};


