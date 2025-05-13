#pragma once
#include "Window.h"
#include "D3Timer.h"
#include "Renderable/Renderable.h"
#include <vector>
#include <memory>

class Application
{
public:
	Application();
	int Run();
private:
	void ProcessFrame();

	Window wnd;
	D3Timer timer;
	std::vector<std::unique_ptr<class Renderable>> renderables;
};
