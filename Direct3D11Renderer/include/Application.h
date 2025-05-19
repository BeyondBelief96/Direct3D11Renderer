#pragma once
#include "Window.h"
#include "D3Timer.h"
#include "Renderable/Renderable.h"
#include "ImGuiManager.h"
#include "Camera/FixedCamera.h"
#include "Camera/FreeFlyCamera.h"
#include "Lights/PointLight.h"
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
	PointLight light;
	std::vector<std::unique_ptr<class Renderable>> renderables;
	std::vector<class Cube*> cubes;
};
