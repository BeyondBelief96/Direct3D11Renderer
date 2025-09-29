#pragma once

#include "Renderable.h"
#include "Bindable/Bindable.h"
#include <DirectXMath.h>

class TestCube : public Renderable
{
public:
	TestCube(Graphics& gfx, float size);
	void SetPos(DirectX::XMFLOAT3 pos) noexcept;
	void SetRotation(float roll, float pitch, float yaw) noexcept;
	DirectX::XMMATRIX GetTransformXM() const noexcept override;
	void SpawnControlWindow(Graphics& gfx, const char* name) noexcept;

private:
	struct PSMaterialConstant
	{
		float specularReflectance = 0.8f;  // 4 bytes
		float specularShininess = 30.0f;   // 4 bytes
		float padding1 = 0.0f;              // 4 bytes
		float padding2 = 0.0f;              // 4 bytes - Total: 16 bytes
	} pmc;
	static_assert(sizeof(PSMaterialConstant) % 16 == 0, "Constant buffer must be multiple of 16 bytes");

	DirectX::XMFLOAT3 pos = { 1.0f, 1.0f, 1.0f };
	float roll = 0.0f;
	float pitch = 0.0f;
	float yaw = 0.0f;

	// Separate bindables for outline effect
	std::vector<std::shared_ptr<Bindable>> outlineEffect;
};