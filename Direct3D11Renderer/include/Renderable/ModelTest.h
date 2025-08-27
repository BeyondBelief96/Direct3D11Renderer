#pragma once

#include "RenderableTestObject.h"

class ModelTest : public RenderableTestObject
{
public:
	ModelTest(Graphics& gfx, std::mt19937& rng,
		std::uniform_real_distribution<float>& adist,
		std::uniform_real_distribution<float>& ddist,
		std::uniform_real_distribution<float>& odist,
		std::uniform_real_distribution<float>& rdist,
		float scale);

	DirectX::XMMATRIX GetTransformXM() const noexcept override;
private:
	struct PSMaterialConstantBuffer
	{
		DirectX::XMFLOAT3 color = { 0.5f, 0.5f, 0.5f };
		float specularIntensity = 0.6f;
		float specularPower = 30.0f;
		float padding[3] = { 0.0f, 0.0f, 0.0f };
	} materialConstantBuffer;
};