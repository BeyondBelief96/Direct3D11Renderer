#include "RenderPass/Job.h"
#include "RenderPass/Step.h"
#include "Renderable/Renderable.h"

Job::Job(const Renderable* pRenderable, const Step* pStep)
	:
	pRenderable(pRenderable),
	pStep(pStep)
{
}

void Job::Execute(Graphics& gfx) const noexcept
{
	pRenderable->Bind(gfx);
	pStep->Bind(gfx);
	gfx.DrawIndexed(pRenderable->GetIndexCount());
}