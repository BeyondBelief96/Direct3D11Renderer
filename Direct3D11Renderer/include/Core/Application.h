#pragma once
#include "Window.h"
#include "Utilities/D3Timer.h"
#include "Renderable/Renderable.h"
#include "Renderable/Model/Model.h"
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
	void RenderPointLightControlWindow() noexcept;

	FreeFlyCamera freeCamera;
	Window wnd;
	D3Timer timer;
	static float ui_speed_factor;
	float speed_factor = 1.0f;
	PointLight light;
	std::unique_ptr<Model> model;
	struct
	{
		float x = 0.0f;
		float y = 0.0f;
		float z = 0.0f;
		float ambient[3] = { 0.05f, 0.05f, 0.05f };
		float diffuse[3] = { 1.0f, 1.0f, 1.0f };
		float diffuseIntensity = 1.0f;
		float attConstant = 1.0f;
		float attLinear = 0.045f;
		float attQuadratic = 0.0075f;
	} lightControls;
};
