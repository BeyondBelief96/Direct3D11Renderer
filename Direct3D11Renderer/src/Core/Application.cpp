#include "Core/Application.h"
#include "imgui/imgui.h"
#include "imgui/imgui_impl_win32.h"
#include "imgui/imgui_impl_dx11.h"

 float Application::ui_speed_factor = 1.0f;

 Application::Application()
     : wnd(1920, 1080, L"D3DEngine"),
     freeCamera({ 0.0f, 0.0f, -30.0f }),
     light(wnd.Gfx())
 {
     std::mt19937 rng(std::random_device{}());

     // Load a single model (adjust path as needed)
     model = std::make_unique<Model>(wnd.Gfx(), "assets/models/nanosuit.obj");

     wnd.Gfx().SetProjection(freeCamera.GetProjectionMatrix(45.0f, 16.0f / 9.0f, 0.5f, 100.0f));
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

    wnd.Gfx().SetProjection(freeCamera.GetProjectionMatrix(45.0f, 16.0f / 9.0f, 0.5f, 100.0f));
    wnd.Gfx().SetView(freeCamera.GetViewMatrix());
    wnd.Gfx().BeginFrame(0.07f, 0.0f, 0.12f);

    using namespace DirectX;
    const auto modelTransform = XMMatrixRotationRollPitchYaw(modelPose.pitch, modelPose.yaw, modelPose.roll) *
        XMMatrixTranslation(modelPose.x, modelPose.y, modelPose.z);

    // UI
    SpawnSimulationWindow();
    SpawnModelWindow();

    // Apply light controls
    light.SetPosition({ lightControls.x, lightControls.y, lightControls.z });
    light.SetAmbient({ lightControls.ambient[0], lightControls.ambient[1], lightControls.ambient[2] });
    light.SetDiffuse({ lightControls.diffuse[0], lightControls.diffuse[1], lightControls.diffuse[2] });
    light.SetDiffuseIntensity(lightControls.diffuseIntensity);
    light.SetAttenuation(lightControls.attConstant, lightControls.attLinear, lightControls.attQuadratic);

    // Bind and render
    light.Bind(wnd.Gfx());
    model->Draw(wnd.Gfx(), modelTransform);
    light.Render(wnd.Gfx());

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

void Application::SpawnModelWindow() noexcept
{
    if (ImGui::Begin("Model"))
    {
        ImGui::Text("Orientation");
        ImGui::SliderAngle("Roll", &modelPose.roll, -180.0f, 180.0f);
        ImGui::SliderAngle("Pitch", &modelPose.pitch, -180.0f, 180.0f);
        ImGui::SliderAngle("Yaw", &modelPose.yaw, -180.0f, 180.0f);

        ImGui::Text("Position");
        ImGui::SliderFloat("X", &modelPose.x, -20.0f, 20.0f, "%.2f");
        ImGui::SliderFloat("Y", &modelPose.y, -20.0f, 20.0f, "%.2f");
        ImGui::SliderFloat("Z", &modelPose.z, -20.0f, 20.0f, "%.2f");

        ImGui::Separator();
        ImGui::Text("Light");
        ImGui::SliderFloat("Light X", &lightControls.x, -50.0f, 50.0f, "%.2f");
        ImGui::SliderFloat("Light Y", &lightControls.y, -50.0f, 50.0f, "%.2f");
        ImGui::SliderFloat("Light Z", &lightControls.z, -50.0f, 50.0f, "%.2f");
        ImGui::ColorEdit3("Ambient", lightControls.ambient);
        ImGui::ColorEdit3("Diffuse", lightControls.diffuse);
        ImGui::SliderFloat("Intensity", &lightControls.diffuseIntensity, 0.0f, 4.0f);
        ImGui::SliderFloat("Att C", &lightControls.attConstant, 0.0f, 2.0f);
        ImGui::SliderFloat("Att L", &lightControls.attLinear, 0.0f, 1.0f);
        ImGui::SliderFloat("Att Q", &lightControls.attQuadratic, 0.0f, 1.0f);
    }
    ImGui::End();
}



