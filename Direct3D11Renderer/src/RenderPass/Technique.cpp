#include "RenderPass/Technique.h"

void Technique::Submit(class FrameManager& frameManager, const class Renderable& renderable) const noexcept
{
	if (active)
	{
		for (const auto& step : steps)
		{
			step.Submit(frameManager, renderable);
		}
	}
}

void Technique::AddStep(Step step) noexcept
{
	steps.push_back(std::move(step));
}

void Technique::Activate() noexcept
{
	active = true;
}

void Technique::Deactivate() noexcept
{
	active = false;
}

void Technique::InitializeParentReferences(const class Renderable& parent) const
{
	for (const auto& step : steps)
	{
		step.InitializeParentReferences(parent);
	}
}