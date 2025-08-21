#pragma once
#include "Window.h"
#include "Utilities/D3Timer.h"
#include "Renderable/Renderable.h"
#include "Renderable/Cube.h"
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

	FreeFlyCamera freeCamera;
	Window wnd;
	D3Timer timer;
	static float ui_speed_factor;
	float speed_factor = 1.0f;
	PointLight light;

	std::vector<std::unique_ptr<Renderable>> renderables;
	std::vector<Cube*> cubes;
	static constexpr size_t nRenderables = 180;

	std::optional<int> comboBoxIndex;
	std::set<int> boxControlIds;
};
