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
		alignas(16)DirectX::XMFLOAT3 lightPos;
		alignas(16)DirectX::XMFLOAT3 materialColor;
		alignas(16)DirectX::XMFLOAT3 ambientColor;
		alignas(16)DirectX::XMFLOAT3 diffuseColor;
		float diffuseIntensity;
		float attConstant;
		float attLinear;
		float attQuadratic;
	};
	PointLightConstantBuffer lightConstantBuffer;
	mutable SolidSphere mesh;
	mutable PixelConstantBuffer<PointLightConstantBuffer> lightPosCBuf;
};