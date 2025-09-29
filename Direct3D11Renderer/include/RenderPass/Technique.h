#pragma once

#include "Step.h"
#include <vector>

class Technique
{
public:
	void Submit(class FrameManager& frameManager, const class Renderable& renderable) const noexcept;
	void AddStep(Step step) noexcept;
	void Activate() noexcept;
	void Deactivate() noexcept;
	void InitializeParentReferences(const class Renderable& renderable) const;
private:
	bool active = true;
	std::vector<Step> steps;

};