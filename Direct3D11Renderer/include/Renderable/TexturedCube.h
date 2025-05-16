#pragma once

#include "RenderableTestObject.h"
#include "Geometry/GeometryFactory.h"
#include <random>
#include <DirectXMath.h>

class TexturedCube : public RenderableTestObject
{
public:
    TexturedCube(
        Graphics& gfx,
        std::mt19937& rng,
        std::uniform_real_distribution<float>& adist,
        std::uniform_real_distribution<float>& ddist,
        std::uniform_real_distribution<float>& odist,
        std::uniform_real_distribution<float>& rdist,
        float size = 1.0f,
        const std::wstring& textureFilename = L""
    );

	DirectX::XMMATRIX GetTransformXM() const noexcept override;
};