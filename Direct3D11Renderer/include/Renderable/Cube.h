#pragma once
#include "Renderable/RenderableTestObject.h"
#include <DirectXMath.h>

class Cube : public RenderableTestObject
{
public:
    Cube(
        Graphics& gfx,
        std::mt19937& rng,
        std::uniform_real_distribution<float>& adist,
        std::uniform_real_distribution<float>& ddist,
        std::uniform_real_distribution<float>& odist,
        std::uniform_real_distribution<float>& rdist,
        std::uniform_real_distribution<float>& bdist,
        DirectX::XMFLOAT3 materialColor
    );

private:
    // Model Transform matrix for special deformation
    DirectX::XMFLOAT3X3 mt;
};