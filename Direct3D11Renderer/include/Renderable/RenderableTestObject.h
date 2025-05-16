#pragma once

#include "Renderable.h"
#include <random>
#include <DirectXMath.h>

class RenderableTestObject : public Renderable
{
public:
    RenderableTestObject(
        std::mt19937& rng,
        std::uniform_real_distribution<float>& adist,  // Angular distribution
        std::uniform_real_distribution<float>& ddist,  // Delta (speed) distribution
        std::uniform_real_distribution<float>& odist,  // Orbital angle distribution
        std::uniform_real_distribution<float>& rdist   // Radius distribution
    );

    // Common update method for all positionable renderables
    void Update(float dt) noexcept override;

    // Common transform matrix calculation
    DirectX::XMMATRIX GetTransformXM() const noexcept override;
protected:
    // Positional
    float r;              // Distance from center
    float roll = 0.0f;    // Roll rotation
    float pitch = 0.0f;   // Pitch rotation
    float yaw = 0.0f;     // Yaw rotation
    float theta;          // Orbital angle 1
    float phi;            // Orbital angle 2
    float chi;            // Orbital angle 3

    // Speed (delta/s)
    float droll;          // Roll rotation speed
    float dpitch;         // Pitch rotation speed
    float dyaw;           // Yaw rotation speed
    float dtheta;         // Theta rotation speed
    float dphi;           // Phi rotation speed
    float dchi;           // Chi rotation speed
};