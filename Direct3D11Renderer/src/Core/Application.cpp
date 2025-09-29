#include "Core/Application.h"
#include "imgui.h"
#include "imgui_impl_win32.h"
#include "imgui_impl_dx11.h"
#include <random>

 float Application::ui_speed_factor = 1.0f;

 Application::Application()
     : wnd(2560, 1440, L"D3DEngine"),
     camera({ 0.0f, 0.0f, -30.0f }),
     light(wnd.Gfx())
 {
     //model = std::make_unique<Model>(wnd.Gfx(), "assets/models/Sponza/sponza.obj", 0.1f);

	 // Create multiple test cubes for better testing
	 testCubes.reserve(3);
	 testCubes.emplace_back(std::make_unique<TestCube>(wnd.Gfx(), 8.0f));
	 testCubes.emplace_back(std::make_unique<TestCube>(wnd.Gfx(), 6.0f));
	 testCubes.emplace_back(std::make_unique<TestCube>(wnd.Gfx(), 4.0f));

	 // Position the cubes
	 testCubes[0]->SetPos({ -15.0f, 0.0f, 0.0f });
	 testCubes[1]->SetPos({ 0.0f, 0.0f, 0.0f });
	 testCubes[2]->SetPos({ 15.0f, 0.0f, 0.0f });

	 // Setup camera for better scene viewing
     camera.SetSpeed(50.0f);
     camera.SetPosition({ 0.0f, 20.0f, -40.0f });

	 // Setup light for better illumination - TEMPORARY: Very bright ambient for debugging
	 light.SetPosition({ 0.0f, 25.0f, -20.0f });
	 light.SetDiffuseIntensity(2.0f);
	 light.SetAmbient({ 0.5f, 0.5f, 0.5f }); // Much brighter ambient to see if texture is there
	 light.SetDiffuse({ 1.0f, 1.0f, 1.0f });
	 light.SetAttenuation(1.0f, 0.01f, 0.0001f); // Reduced attenuation
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

    camera.ProcessInput(wnd, wnd.mouse, wnd.kbd, dt);

    wnd.Gfx().SetProjection(camera.GetProjectionMatrix(wnd.GetWidth(), wnd.GetHeight()));
    wnd.Gfx().SetView(camera.GetViewMatrix());
    wnd.Gfx().BeginFrame(0.07f, 0.0f, 0.12f);

    // UI
    SpawnSimulationWindow();
    light.SpawnControlWindow();
    //model->ShowModelControlWindow();

	// Show UI for each cube
	for (size_t i = 0; i < testCubes.size(); ++i)
	{
		std::string windowName = "Test Cube " + std::to_string(i + 1);
		testCubes[i]->SpawnControlWindow(wnd.Gfx(), windowName.c_str());
	}

    // Bind and render
	light.Bind(wnd.Gfx());  // Bind light constants globally for all pixel shaders
	light.Submit(frameManager);

	// Submit all cubes for rendering
	for (auto& cube : testCubes)
	{
		cube->Bind(wnd.Gfx());
		cube->Submit(frameManager);
	}

    frameManager.Excecute(wnd.Gfx());

    wnd.Gfx().EndFrame();
    frameManager.Reset();
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




