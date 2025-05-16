#pragma once

#include "Renderable/RenderableTestObject.h"
#include "Geometry/GeometryFactory.h"
#include "Bindable/BindableBase.h"
#include <random>

class Sphere : public RenderableTestObject
{
public:
    Sphere(
        Graphics& gfx,
        std::mt19937& rng,
        std::uniform_real_distribution<float>& adist,
        std::uniform_real_distribution<float>& ddist,
        std::uniform_real_distribution<float>& odist,
        std::uniform_real_distribution<float>& rdist,
        float radius = 1.0f,
        int tessellation = 16
    );

    DirectX::XMMATRIX GetTransformXM() const noexcept override;
};
