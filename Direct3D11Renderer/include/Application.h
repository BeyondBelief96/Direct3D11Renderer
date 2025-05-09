#pragma once
#include "Window.h"

class Application
{
public:
	Application();
	int Run();
private:
	void ProcessFrame();

	Window wnd;
};
