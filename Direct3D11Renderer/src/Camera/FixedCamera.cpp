#include "Camera/FixedCamera.h"
#include "imgui.h"

DirectX::XMMATRIX FixedCamera::GetViewMatrix() const noexcept
{
	const auto position = DirectX::XMVector3Transform(
		DirectX::XMVectorSet(0.0f, 0.0f, -r, 0.0f),
		DirectX::XMMatrixRotationRollPitchYaw(theta, phi, 0.0f)
	);

	return DirectX::XMMatrixLookAtLH(
		position, DirectX::XMVectorZero(),
		DirectX::XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f)
	) * DirectX::XMMatrixRotationRollPitchYaw(
		pitch, -yaw, roll);
}

void FixedCamera::SpawnControlWindow() noexcept
{
	if (ImGui::Begin("Camera"))
	{
		ImGui::Text("Position");
		ImGui::SliderFloat("R", &r, 1.0f, 80.0f, "%.1f");
		ImGui::SliderAngle("Theta", &theta, -89.0f, 89.0f);
		ImGui::SliderAngle("Phi", &phi, -180.0f, 180.0f);
		ImGui::Text("Orientation");
		ImGui::SliderAngle("Roll", &roll, -180.0f, 180.0f);
		ImGui::SliderAngle("Pitch", &pitch, -180.0f, 180.0f);
		ImGui::SliderAngle("Yaw", &yaw, -180.0f, 180.0f);
		if (ImGui::Button("Reset"))
		{
			Reset();
		}
	}
	ImGui::End();
}

void FixedCamera::Reset() noexcept
{
	r = 20.0f;
	theta = 0.0f;
	phi = 0.0f;
	pitch = 0.0f;
	yaw = 0.0f;
	roll = 0.0f;
}
