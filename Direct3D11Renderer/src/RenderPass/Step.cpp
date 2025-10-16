#include "RenderPass/Step.h"
#include "RenderPass/FrameManager.h"	
#include "RenderPass/Job.h"

Step::Step(size_t targetPass_in)
	:
	targetPass(targetPass_in)
{
}

void Step::AddBindable(std::shared_ptr<Bindable> bindable) noexcept
{
	bindables.push_back(std::move(bindable));
}

void Step::Submit(FrameManager& frameManager, const class Renderable& renderable) const
{
	frameManager.Accept(Job{ &renderable, this }, targetPass);
}

void Step::Bind(Graphics& gfx) const
{
	for (const auto& b : bindables)
	{
		b->Bind(gfx);
	}
}

void Step::Accept(TechniqueProbe& probe)
{
	for (const auto& b : bindables)
	{
		b->Accept(probe);
	}
}

void Step::InitializeParentReferences(const class Renderable& parent) const
{
	for (const auto& b : bindables)
	{
		b->InitializeParentReference(parent);
	}
}