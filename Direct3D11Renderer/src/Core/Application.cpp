#include "Core/Application.h"
#include "imgui/imgui.h"
#include "imgui/imgui_impl_win32.h"
#include "imgui/imgui_impl_dx11.h"
#include <random>

 float Application::ui_speed_factor = 1.0f;

 Application::Application()
     : wnd(2560, 1440, L"D3DEngine"),
     freeCamera({ 0.0f, 0.0f, -30.0f }),
     light(wnd.Gfx()),
     plane(wnd.Gfx(), 3.0f)
 {
     std::mt19937 rng(std::random_device{}());

     // Load a single model (adjust path as needed)
     model = std::make_unique<Model>(wnd.Gfx(), "assets/models/nano_textured/nanosuit.obj");

     wnd.Gfx().SetProjection(CreateProjectionMatrix(freeCamera, 45.0f, 16.0f / 9.0f, 0.5f, 100.0f));
	 
 }

int Application::Run()
{
	while (true)
	{
		if (auto ecode = Window::ProcessMessages())
		{
			return *ecode;
		}

		ProcessFrame();
	}
}

void Application::ProcessFrame()
{
    if (wnd.kbd.KeyIsPressed(VK_SPACE))
    {
        speed_factor = 0.0f;
    }
    else
    {
        speed_factor = ui_speed_factor;
    }

    auto dt = timer.Mark();

    freeCamera.ProcessInput(wnd, wnd.mouse, wnd.kbd, dt);

    wnd.Gfx().SetProjection(CreateProjectionMatrix(freeCamera, 45.0f, 16.0f / 9.0f, 0.5f, 100.0f));
    wnd.Gfx().SetView(freeCamera.GetViewMatrix());
    wnd.Gfx().BeginFrame(0.07f, 0.0f, 0.12f);

    // UI
    SpawnSimulationWindow();
	RenderPointLightControlWindow();
    model->ShowModelControlWindow();
	plane.SpawnControlWindow(wnd.Gfx());

    // Apply light controls
    light.SetPosition({ lightControls.x, lightControls.y, lightControls.z });
    light.SetAmbient({ lightControls.ambient[0], lightControls.ambient[1], lightControls.ambient[2] });
    light.SetDiffuse({ lightControls.diffuse[0], lightControls.diffuse[1], lightControls.diffuse[2] });
    light.SetDiffuseIntensity(lightControls.diffuseIntensity);
    light.SetAttenuation(lightControls.attConstant, lightControls.attLinear, lightControls.attQuadratic);

    // Bind and render
    light.Bind(wnd.Gfx());
    model->Render(wnd.Gfx());
    light.Render(wnd.Gfx());
	plane.Render(wnd.Gfx());

    wnd.Gfx().EndFrame();
}

void Application::SpawnSimulationWindow() noexcept
{
    if (ImGui::Begin("Simulation Speed"))
    {
        if (ImGui::SliderFloat("Speed Factor", &ui_speed_factor, 0.0f, 4.0f))
        {
            if (!wnd.kbd.KeyIsPressed(VK_SPACE))
            {
                speed_factor = ui_speed_factor;
            }
        }

        ImGui::Text("Application Average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate,
            ImGui::GetIO().Framerate);
        ImGui::Text("Status: %s", wnd.kbd.KeyIsPressed(VK_SPACE) ? "PAUSED" : "RUNNING (hold spacebar to pause)");
    }
    ImGui::End();
}

void Application::RenderPointLightControlWindow() noexcept
{
    if (ImGui::Begin("Point Light Controls"))
    {
        ImGui::SliderFloat3("Position", &lightControls.x, -50.0f, 50.0f);
        ImGui::ColorEdit3("Ambient", lightControls.ambient);
        ImGui::ColorEdit3("Diffuse", lightControls.diffuse);
        ImGui::SliderFloat("Diffuse Intensity", &lightControls.diffuseIntensity, 0.0f, 5.0f);
        ImGui::SliderFloat("Attenuation Constant", &lightControls.attConstant, 0.0f, 5.0f);
        ImGui::SliderFloat("Attenuation Linear", &lightControls.attLinear, 0.0f, 1.0f);
        ImGui::SliderFloat("Attenuation Quadratic", &lightControls.attQuadratic, 0.0f, 1.0f);
    }
	ImGui::End();
}



