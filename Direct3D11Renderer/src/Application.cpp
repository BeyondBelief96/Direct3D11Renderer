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
}


