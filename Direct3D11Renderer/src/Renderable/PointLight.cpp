#include "Renderable/PointLight.h"
#include "RenderPass/FrameManager.h"
#include "imgui.h"

PointLight::PointLight(Graphics& gfx, float radius) :
	mesh(gfx, radius),
	lightPosCBuf(gfx)
{
	Reset();
}

void PointLight::SpawnControlWindow() noexcept
{
	if (ImGui::Begin("Light"))
	{
		DrawControlsInline();
	}
	ImGui::End();
}

void PointLight::DrawControlsInline() noexcept
{
	ImGui::Text("Position");
	ImGui::SliderFloat("World X (+Right)", &lightConstantBuffer.lightPos.x, -300.0f, 300.0f, "%.1f");
	ImGui::SliderFloat("World Y (+Up)", &lightConstantBuffer.lightPos.y, -300.0f, 300.0f, "%.1f");
	ImGui::SliderFloat("World Z (+Forward)", &lightConstantBuffer.lightPos.z, -300.0f, 300.0f, "%.1f");

	ImGui::Text("Intensity/Color");
	ImGui::SliderFloat("Intensity", &lightConstantBuffer.diffuseIntensity, 0.01f, 2.0f, "%.2f");
	ImGui::ColorEdit3("Diffuse Color", &lightConstantBuffer.diffuseColor.x);
	ImGui::ColorEdit3("Ambient", &lightConstantBuffer.ambientColor.x);

	ImGui::Text("Falloff");
	ImGui::SliderFloat("Constant", &lightConstantBuffer.attConstant, 0.1f, 3.0f, "%.2f");
	ImGui::SliderFloat("Linear", &lightConstantBuffer.attLinear, 0.001f, 0.5f, "%.4f", ImGuiSliderFlags_Logarithmic);
	ImGui::SliderFloat("Quadratic", &lightConstantBuffer.attQuadratic, 0.0001f, 0.5f, "%.5f", ImGuiSliderFlags_Logarithmic);

	if (ImGui::Button("Reset"))
	{
		Reset();
	}
}

void PointLight::Reset() noexcept
{
	lightConstantBuffer =
	{
		{0.0f, 0.0f, 0.0f},
		{0.05f, 0.05f, 0.05f},
		{1.0f, 1.0f, 1.0f},
		1.0f,
		1.0f,
		0.045f,
		0.0075f
	};
}

void PointLight::Bind(Graphics& gfx) const noexcept
{
	// Create a copy of the light buffer
	PointLightConstantBuffer lightBuffer = lightConstantBuffer;

	DirectX::XMVECTOR lightPosWorld = DirectX::XMLoadFloat3(&lightConstantBuffer.lightPos);
	DirectX::XMMATRIX viewMatrix = gfx.GetView();
	DirectX::XMVECTOR lightPosView = DirectX::XMVector3Transform(lightPosWorld, viewMatrix);
	DirectX::XMStoreFloat3(&lightBuffer.lightPos, lightPosView);

	// Update the constant buffer with view-space position
	lightPosCBuf.Update(gfx, lightBuffer);
	lightPosCBuf.Bind(gfx);
}

void PointLight::Submit(FrameManager& frameManager) const noexcept(!_DEBUG)
{
	mesh.SetPosition(lightConstantBuffer.lightPos);
	mesh.Submit(frameManager);
}
