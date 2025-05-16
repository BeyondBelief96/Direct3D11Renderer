#include "Renderable/RenderableTestObject.h"

RenderableTestObject::RenderableTestObject(
    std::mt19937& rng,
    std::uniform_real_distribution<float>& adist,
    std::uniform_real_distribution<float>& ddist,
    std::uniform_real_distribution<float>& odist,
    std::uniform_real_distribution<float>& rdist
) :
    r(rdist(rng)),
    roll(0.0f),
    pitch(0.0f),
    yaw(0.0f),
    theta(adist(rng)),
    phi(adist(rng)),
    chi(adist(rng)),
    droll(ddist(rng)),
    dpitch(ddist(rng)),
    dyaw(ddist(rng)),
    dtheta(odist(rng)),
    dphi(odist(rng)),
    dchi(odist(rng))
{
}

void RenderableTestObject::Update(float dt) noexcept
{
    roll += droll * dt;
    pitch += dpitch * dt;
    yaw += dyaw * dt;
    theta += dtheta * dt;
    phi += dphi * dt;
    chi += dchi * dt;
}

DirectX::XMMATRIX RenderableTestObject::GetTransformXM() const noexcept
{
    return DirectX::XMMatrixRotationRollPitchYaw(pitch, yaw, roll) *
        DirectX::XMMatrixTranslation(r, 0.0f, 0.0f) *
        DirectX::XMMatrixRotationRollPitchYaw(theta, phi, chi);
}