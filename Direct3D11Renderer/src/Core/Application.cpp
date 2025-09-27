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
     model = std::make_unique<Model>(wnd.Gfx(), "assets/models/Sponza/sponza.obj", 0.1f);
	 testCube = std::make_unique<TestCube>(wnd.Gfx(), 10.0f);
     camera.SetSpeed(50.0f);
     camera.SetPosition({ 0.0f, 30.0f, 0.0f });
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
    model->ShowModelControlWindow();
	testCube->SpawnControlWindow(wnd.Gfx(), "Test Outline Cube");


    // Bind and render
    light.Bind(wnd.Gfx());
    model->Render(wnd.Gfx());
    light.Render(wnd.Gfx());
    testCube->Render(wnd.Gfx());
    testCube->RenderOutline(wnd.Gfx());

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




