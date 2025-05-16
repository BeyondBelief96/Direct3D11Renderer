#pragma once

#include "Graphics.h"
#include "SolidSphere.h"
#include "ConstantBuffer.h"

class PointLight
{
public:
	PointLight(Graphics& gfx, float radius = 0.5f);
	void SpawnControlWindow() noexcept;
	void Reset() noexcept;
	void Render(Graphics& gfx) const noexcept (!_DEBUG);
	void Bind(Graphics& gfx) const noexcept;
private:
	struct PointLightConstantBuffer
	{
		DirectX::XMFLOAT3 lightPos;
		float padding;
	};
	DirectX::XMFLOAT3 position = { 0.0f, 0.0f, 0.0f };
	mutable SolidSphere mesh;
	mutable PixelConstantBuffer<PointLightConstantBuffer> lightPosCBuf;
};