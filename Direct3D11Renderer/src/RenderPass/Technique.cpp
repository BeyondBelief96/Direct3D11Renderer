#include "RenderPass/Technique.h"

Technique::Technique(std::string name) noexcept : name(name)
{
}

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

bool Technique::IsActive() const noexcept
{
	return active;
}

void Technique::Accept(TechniqueProbe& probe)
{
	probe.SetTechnique(this);
	for (auto& step : steps)
	{
		step.Accept(probe);
	}
}

const std::string& Technique::GetName() const noexcept
{
	return name;
}

void Technique::SetActiveState(bool state) noexcept
{
	active = state;
}

void Technique::InitializeParentReferences(const class Renderable& parent) const
{
	for (const auto& step : steps)
	{
		step.InitializeParentReferences(parent);
	}
}