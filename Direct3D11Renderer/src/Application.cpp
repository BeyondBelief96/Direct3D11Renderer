#include "Application.h"


Application::Application() : wnd(800, 600, L"D3DEngine")
{
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
	const float c = sin(timer.Peek()) / 2.0f + 0.5f;
	wnd.Gfx().ClearBuffer(c, c, 1.0f);

	wnd.Gfx().DrawTestTriangle();
	wnd.Gfx().EndFrame();
}


