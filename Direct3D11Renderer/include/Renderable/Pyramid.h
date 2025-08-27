#pragma once

#include "RenderableTestObject.h"

class Pyramid : public RenderableTestObject
{
public:
    Pyramid(
        Graphics& gfx,
        std::mt19937& rng,
        std::uniform_real_distribution<float>& adist,
        std::uniform_real_distribution<float>& ddist,
        std::uniform_real_distribution<float>& odist,
        std::uniform_real_distribution<float>& rdist,
        float radius = 1.0f,
        float height = 2.0f,
        int sides = 4);

    DirectX::XMMATRIX GetTransformXM() const noexcept override;
};