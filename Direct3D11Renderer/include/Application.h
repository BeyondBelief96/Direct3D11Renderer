#pragma once
#include "Window.h"
#include "D3Timer.h"

class Application
{
public:
	Application();
	int Run();
private:
	void ProcessFrame();

	Window wnd;
	D3Timer timer;
};
