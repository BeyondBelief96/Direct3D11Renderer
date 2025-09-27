#pragma once
#include "Window.h"
#include "Utilities/D3Timer.h"
#include "Renderable/Renderable.h"
#include "Renderable/Model/Model.h"
#include "Renderable/TestPlane.h"
#include "Renderable/TestCube.h"
#include "Camera/FreeFlyCamera.h"
#include "Lights/PointLight.h"
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
	static float ui_speed_factor;
	float speed_factor = 1.0f;
	PointLight light;
	std::unique_ptr<Model> model;
	std::unique_ptr<TestCube> testCube;
};
