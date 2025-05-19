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
#include <set>

class Application
{
public:
	Application();
	int Run();
private:
	void ProcessFrame();
	void SpawnSimulationWindow() noexcept;
	void SpawnCubeWindowSelector() noexcept;
	void SpawnCubeWindows() noexcept;

	ImGuiManager imgui;
	//FixedCamera camera;
	FreeFlyCamera freeCamera;
	Window wnd;
	D3Timer timer;
	static float ui_speed_factor;
	float speed_factor = 1.0f;
	PointLight light;

	std::vector<std::unique_ptr<class Renderable>> renderables;
	std::vector<class Cube*> cubes;
	static constexpr size_t nRenderables = 180;

	std::optional<int> comboBoxIndex;
	std::set<int> boxControlIds;
};
