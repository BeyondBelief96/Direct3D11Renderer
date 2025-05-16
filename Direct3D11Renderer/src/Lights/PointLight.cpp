#include "Lights/PointLight.h"
#include "imgui.h"

PointLight::PointLight(Graphics& gfx, float radius) :
	mesh(gfx, radius),
	lightPosCBuf(gfx)
{
	
}

void PointLight::SpawnControlWindow() noexcept
{
	if (ImGui::Begin("Light"))
	{
		ImGui::Text("Position");
		ImGui::SliderFloat("X", &position.x, -60.0f, 60.0f, "%.1f");
		ImGui::SliderFloat("Y", &position.y, -60.0f, 60.0f, "%.1f");
		ImGui::SliderFloat("Z", &position.z, -60.0f, 60.0f, "%.1f");
		if (ImGui::Button("Reset"))
		{
			Reset();
		}
	}
	ImGui::End();
}

void PointLight::Reset() noexcept
{
	position = { 0.0f, 0.0f, 0.0f };
}

void PointLight::Render(Graphics& gfx) const noexcept(!_DEBUG)
{
	mesh.SetPosition(position);
	mesh.Render(gfx);
}

void PointLight::Bind(Graphics& gfx) const noexcept
{
	lightPosCBuf.Update(gfx, PointLightConstantBuffer{ position });
	lightPosCBuf.Bind(gfx);
}
