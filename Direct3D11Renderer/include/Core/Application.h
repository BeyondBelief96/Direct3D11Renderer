#pragma once
#include "Window.h"
#include "Utilities/D3Timer.h"
#include "Renderable/Renderable.h"
#include "Renderable/Model/Model.h"
#include "Renderable/TestCube.h"
#include "Camera/FreeFlyCamera.h"
#include "Renderable/PointLight.h"
#include <vector>
#include <memory>
#include <set>

class Application
{
public:
	Application();
	int Run();
private:
	void ProcessFrame();
	void SpawnSimulationWindow() noexcept;

	FreeFlyCamera camera;
	Window wnd;
	D3Timer timer;
	FrameManager frameManager;
	static float ui_speed_factor;
	float speed_factor = 1.0f;
	PointLight light;
	//std::unique_ptr<Model> model;
	std::vector<std::unique_ptr<TestCube>> testCubes;
};
