#pragma once

#include "Graphics/Graphics.h"
#include "Renderable/SolidSphere.h"
#include "Bindable/ConstantBuffer.h"

class PointLight
{
public:
	PointLight(Graphics& gfx, float radius = 0.5f);
	void SpawnControlWindow() noexcept;
	// Draw controls inline in the current ImGui window (no Begin/End)
	void DrawControlsInline() noexcept;
	void Reset() noexcept;
	void Render(Graphics& gfx) const noexcept (!_DEBUG);
	void Bind(Graphics& gfx) const noexcept;
	// Setters used by Application UI
	void SetPosition(const DirectX::XMFLOAT3& pos) noexcept { lightConstantBuffer.lightPos = pos; }
	void SetAmbient(const DirectX::XMFLOAT3& amb) noexcept { lightConstantBuffer.ambientColor = amb; }
	void SetDiffuse(const DirectX::XMFLOAT3& dif) noexcept { lightConstantBuffer.diffuseColor = dif; }
	void SetDiffuseIntensity(float i) noexcept { lightConstantBuffer.diffuseIntensity = i; }
	void SetAttenuation(float c, float l, float q) noexcept { lightConstantBuffer.attConstant = c; lightConstantBuffer.attLinear = l; lightConstantBuffer.attQuadratic = q; }
	DirectX::XMFLOAT3 GetPosition() const noexcept { return lightConstantBuffer.lightPos; }
private:
	struct PointLightConstantBuffer
	{
		alignas(16)DirectX::XMFLOAT3 lightPos;
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