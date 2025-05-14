#include "Application.h"
#include "Renderable/Cube.h"
#include "Renderable/Sphere.h"
#include "Renderable/Pyramid.h"
#include "Renderable/Plane.h"
#include "Renderable/TexturedCube.h"
#include "imgui.h"
#include "imgui_impl_win32.h"
#include "imgui_impl_dx11.h"


Application::Application() : wnd(1920, 1080, L"D3DEngine")
{
	std::mt19937 rng(std::random_device{}());
	std::uniform_real_distribution<float> adist(0.0f, 3.1415f * 2.0f);
	std::uniform_real_distribution<float> ddist(0.0f, 3.1415f * 2.0f);
	std::uniform_real_distribution<float> odist(0.0f, 3.1415f * 0.3f);
	std::uniform_real_distribution<float> rdist(6.0f, 20.0f);
	std::uniform_real_distribution<float> bdist(0.4f, 3.0f);
	std::uniform_real_distribution<float> sdist(0.8f, 1.5f);
    // Create a mix of geometry types
    for (int i = 0; i < 20; i++)
    {
        // Create boxes
        renderables.push_back(std::make_unique<TexturedCube>(
            wnd.Gfx(),
            rng,
            adist,
            ddist,
            odist,
            rdist,
            3.0f,
            L"assets/cube.png" 
        ));

        // Create spheres
        renderables.push_back(std::make_unique<Sphere>(
            wnd.Gfx(), rng, adist, ddist, odist, rdist, 1.0f, 12
        ));

        //// Create pyramids with various sides
        const int sides[] = { 3, 4, 5, 6 };
        const float heights[] = { 2.0f, 2.5f, 3.0f, 3.5f };

        for (int j = 0; j < 4; j++)
        {
            renderables.push_back(std::make_unique<Pyramid>(
                wnd.Gfx(), rng, adist, ddist, odist, rdist,
                1.0f + (float)j * 0.3f,  // Varying radius
                heights[j],              // Varying height
                sides[j]                 // Varying number of sides
            ));
        }

        //// Create planes with different tessellations
        renderables.push_back(std::make_unique<Plane>(
            wnd.Gfx(), rng, adist, ddist, odist, rdist,
            2.0f + (float)i * 0.2f,   // Width
            2.0f + (float)i * 0.1f,   // Height
            1 + i,                    // Divisions X
            1 + i                     // Divisions Y
        ));
    }
	wnd.Gfx().SetProjection(DirectX::XMMatrixPerspectiveLH(1.0f, 3.0f / 4.0f, 0.5f, 40.0f));
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
	auto dt = timer.Mark() * speed_factor;
    
	wnd.Gfx().BeginFrame(0.07f, 0.0f, 0.12f);

	for (auto& box : renderables)
	{
		box->Update(dt);
		box->Render(wnd.Gfx());
	}

    static char buffer[1024];

    if (ImGui::Begin("Simulation Speed"))
    {
        ImGui::SliderFloat("Speed Factor", &speed_factor, 0.0f, 4.0f);
		ImGui::Text("Application Average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate,
			ImGui::GetIO().Framerate);
        ImGui::InputText("Butts", buffer, sizeof(buffer));
    }
    ImGui::End();

	wnd.Gfx().EndFrame();
}


