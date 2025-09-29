#pragma once
#include <vector>
#include <memory>
#include "Bindable/Bindable.h"
#include "Core/Graphics.h"

class Step
{
public:
	Step(size_t targetPass_in);
	void AddBindable(std::shared_ptr<Bindable> bindable) noexcept;
	void Submit(class FrameManager& frameManager, const class Renderable& renderable) const;
	void Bind(Graphics& gfx) const;
	void InitializeParentReferences(const class Renderable& parent) const;
private:
	size_t targetPass;
	std::vector<std::shared_ptr<Bindable>> bindables;
};