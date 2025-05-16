#pragma once
#include "Renderable/RenderableTestObject.h"
#include "Geometry/GeometryFactory.h"
#include "Bindable/BindableBase.h"
#include <random>

class Plane : public RenderableTestObject
{
public:
    Plane(
        Graphics& gfx,
        std::mt19937& rng,
        std::uniform_real_distribution<float>& adist,
        std::uniform_real_distribution<float>& ddist,
        std::uniform_real_distribution<float>& odist,
        std::uniform_real_distribution<float>& rdist,
        float width = 2.0f,
        float height = 2.0f,
        int divisionsX = 10,
        int divisionsY = 10
    );

    DirectX::XMMATRIX GetTransformXM() const noexcept override;
};