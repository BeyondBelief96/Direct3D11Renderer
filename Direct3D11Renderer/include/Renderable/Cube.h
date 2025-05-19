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

    DirectX::XMMATRIX GetTransformXM() const noexcept override;

    bool SpawnControlWindow(int id, Graphics& gfx) noexcept;
private:
    void SyncMaterial(Graphics& gfx) noexcept (!_DEBUG);

private:
    struct PSMaterialConstantBuffer
    {
        DirectX::XMFLOAT3 color;
        float specularIntensity = 128.0f;
        float specularPower = 30.0f;
        float padding[3] = { 0.0f, 0.0f, 0.0f };
    } materialConstantBuffer;
    // Model Transform matrix for special deformation
    DirectX::XMFLOAT3X3 mt;
};