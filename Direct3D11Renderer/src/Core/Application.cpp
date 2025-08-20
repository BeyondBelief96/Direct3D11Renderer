#include "Core/Application.h"
#include "Renderable/Cube.h"
#include "Renderable/Sphere.h"
#include "Renderable/TexturedCube.h"
#include "Renderable/Pyramid.h"
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
    std::uniform_real_distribution<float> adist(0.0f, 3.1415f * 2.0f);
    std::uniform_real_distribution<float> ddist(0.0f, 3.1415f * 2.0f);
    std::uniform_real_distribution<float> odist(0.0f, 3.1415f * 0.3f);
    std::uniform_real_distribution<float> rdist(6.0f, 20.0f);
    std::uniform_real_distribution<float> bdist(0.0f, 1.0f);
    std::uniform_real_distribution<float> color_dist(0.0f, 1.0f);

    // Distribution for Z positions (positive values)
    std::uniform_real_distribution<float> zdist(10.0f, 50.0f);
    // Distribution for X and Y positions (both positive and negative)
    std::uniform_real_distribution<float> xydist(-20.0f, 20.0f);

    // Create a mix of geometry types
    for (int i = 0; i < 7; i++)
    {
        // Randomize the material color
        DirectX::XMFLOAT3 materialColor(
            color_dist(rng),
            color_dist(rng),
            color_dist(rng));

        // Create boxes with explicit position control
        auto cube = std::make_unique<Cube>(
            wnd.Gfx(),
            rng,
            adist,
            ddist,
            odist,
            rdist,
            bdist,
            materialColor);

        renderables.push_back(std::move(cube));

        // Add a sphere
        auto sphere = std::make_unique<Sphere>(
            wnd.Gfx(),
            rng,
            adist,
            ddist,
            odist,
            rdist,
            1.0f,     // radius
            16        // tessellation
        );

        renderables.push_back(std::move(sphere));

        // Add a pyramid
        auto pyramid = std::make_unique<Pyramid>(
            wnd.Gfx(),
            rng,
            adist,
            ddist,
            odist,
            rdist,
            1.0f,     // radius
            2.0f,     // height
            4         // sides (square base)
        );

        renderables.push_back(std::move(pyramid));
    }

    // Add a few textured cubes
    for (int i = 0; i < 5; i++)
    {
        auto texturedCube = std::make_unique<TexturedCube>(
            wnd.Gfx(),
            rng,
            adist,
            ddist,
            odist,
            rdist,
            1.0f,
            L"assets/kappa50.png"
        );

        renderables.push_back(std::move(texturedCube));
    }

    // Initialize vector of non-owning pointers to cubes
    for (auto& renderable : renderables)
    {
        if (auto cp = dynamic_cast<Cube*>(renderable.get()))
        {
            cubes.push_back(cp);
        }
    }

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
    // Check if space is pressed and update speed_factor accordingly
    if (wnd.kbd.KeyIsPressed(VK_SPACE))
    {
        speed_factor = 0.0f; // Set to 0 when space is pressed
    }
    else
    {
        speed_factor = ui_speed_factor; // Restore to UI value when not pressed
    }

    auto dt = timer.Mark();

    freeCamera.ProcessInput(wnd, wnd.mouse, wnd.kbd, dt);

    wnd.Gfx().SetProjection(freeCamera.GetProjectionMatrix(45.0f, 16.0f / 9.0f, 0.5f, 100.0f));
    wnd.Gfx().SetView(freeCamera.GetViewMatrix());
    wnd.Gfx().BeginFrame(0.07f, 0.0f, 0.12f);
    light.Bind(wnd.Gfx());


    for (auto& box : renderables)
    {
        box->Update(dt * speed_factor);
        box->Render(wnd.Gfx());
    }
	light.Render(wnd.Gfx());

    static char buffer[1024];

    SpawnSimulationWindow();
    light.SpawnControlWindow();
    SpawnCubeWindowSelector();
    SpawnCubeWindows();

    wnd.Gfx().EndFrame();
}

void Application::SpawnSimulationWindow() noexcept
{
    if (ImGui::Begin("Simulation Speed"))
    {
        if (ImGui::SliderFloat("Speed Factor", &ui_speed_factor, 0.0f, 4.0f))
        {
            // Only update the actual speed factor if space is not pressed
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

void Application::SpawnCubeWindowSelector() noexcept
{
    if (ImGui::Begin("Boxes"))
    {
        using namespace std::string_literals;

        // Create a safe preview text that doesn't access the optional when it's empty
        const std::string preview = comboBoxIndex.has_value()
            ? std::to_string(comboBoxIndex.value())
            : "Choose a box..."s;

        if (ImGui::BeginCombo("Box Number", preview.c_str()))
        {
            for (int i = 0; i < cubes.size(); i++)
            {
                // Check if comboBoxIndex has a value before comparing
                const bool selected = comboBoxIndex.has_value() && comboBoxIndex.value() == i;

                if (ImGui::Selectable(std::to_string(i).c_str(), selected))
                {
                    comboBoxIndex = i;
                }

                if (selected)
                {
                    ImGui::SetItemDefaultFocus();
                }
            }
            ImGui::EndCombo();
        }

        // Only enable the button if comboBoxIndex has a value
        if (comboBoxIndex.has_value())
        {
            if (ImGui::Button("Spawn Control Window"))
            {
                boxControlIds.insert(comboBoxIndex.value());
                comboBoxIndex.reset();
            }
        }
        else
        {
            // Disabled button when no box is selected
            ImGui::BeginDisabled();
            ImGui::Button("Spawn Control Window");
            ImGui::EndDisabled();
            ImGui::TextColored(ImVec4(1.0f, 0.5f, 0.0f, 1.0f), "Select a box first");
        }
    }
    ImGui::End();
}

void Application::SpawnCubeWindows() noexcept
{
    for (auto i = boxControlIds.begin(); i != boxControlIds.end(); )
    {
        if (!cubes[*i]->SpawnControlWindow(*i, wnd.Gfx()))
        {
            i = boxControlIds.erase(i);
        }
        else
        {
            i++;
        }
    }
}



