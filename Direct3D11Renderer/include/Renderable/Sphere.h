#pragma once

#include "Renderable.h"
#include "Geometry/GeometryFactory.h"
#include "Bindable/BindableBase.h"
#include <random>

class Sphere : public Renderable
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

    void Update(float dt) noexcept override;
    DirectX::XMMATRIX GetTransformXM() const noexcept override;
private:
    // Positional
    float r;
    float roll = 0.0f;
    float pitch = 0.0f;
    float yaw = 0.0f;
    float theta;
    float phi;
    float chi;

    // Speed (delta/s)
    float droll;
    float dpitch;
    float dyaw;
    float dtheta;
    float dphi;
    float dchi;
};
