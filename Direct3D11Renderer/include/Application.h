#pragma once
#include "Window.h"
#include "D3Timer.h"
#include "Renderable/Renderable.h"
#include "ImGuiManager.h"
#include "Camera/FixedCamera.h"
#include "Camera/FreeFlyCamera.h"
#include <vector>
#include <memory>

class Application
{
public:
	Application();
	int Run();
private:
	void ProcessFrame();

	ImGuiManager imgui;
	//FixedCamera camera;
	FreeFlyCamera freeCamera;
	Window wnd;
	D3Timer timer;
	float speed_factor = 1.0f;
	std::vector<std::unique_ptr<class Renderable>> renderables;
};
