#include "Application.h"


Application::Application() : wnd(800, 600, L"D3DEngine")
{
	std::mt19937 rng(std::random_device{}());
	std::uniform_real_distribution<float> adist(0.0f, 3.1415f * 2.0f);
	std::uniform_real_distribution<float> ddist(0.0f, 3.1415f * 2.0f);
	std::uniform_real_distribution<float> odist(0.0f, 3.1415f * 0.3f);
	std::uniform_real_distribution<float> rdist(6.0f, 20.0f);
	std::uniform_real_distribution<float> bdist(0.4f, 3.0f);
	for (auto i = 0; i < 500; i++)
	{
		cubes.push_back(std::make_unique<Cube>(
			wnd.Gfx(), rng, adist,
			ddist, odist, rdist, bdist
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
	auto dt = timer.Mark();
	wnd.Gfx().ClearBuffer(0.07f, 0.0f, 0.12f);
	for (auto& box : cubes)
	{
		box->Update(dt);
		box->Render(wnd.Gfx());
	}
	wnd.Gfx().EndFrame();
}


