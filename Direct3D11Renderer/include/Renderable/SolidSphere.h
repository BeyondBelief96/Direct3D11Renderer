#pragma once

#include "Renderable.h"

class SolidSphere : public Renderable
{
public:
	SolidSphere(Graphics& gfx, float radius);
	void Update(float dt) noexcept override;
	void SetPosition(DirectX::XMFLOAT3 pos) noexcept;
	DirectX::XMMATRIX GetTransformXM() const noexcept override;
private:
	DirectX::XMFLOAT3 position = { 1.0f, 1.0f, 1.0f };
};
