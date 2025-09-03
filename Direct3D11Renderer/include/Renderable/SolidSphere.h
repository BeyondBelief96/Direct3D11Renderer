#pragma once

#include "Renderable.h"

class SolidSphere : public Renderable
{
public:
	SolidSphere(Graphics& gfx, float radius);
	void SetPosition(DirectX::XMFLOAT3 pos) noexcept;
	DirectX::XMMATRIX GetTransformXM() const noexcept override;
private:
	DirectX::XMFLOAT3 position = { 2.6f, 13.5f, -3.5f };
};
