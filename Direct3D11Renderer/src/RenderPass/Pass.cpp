#include "RenderPass/Pass.h"

void Pass::Accept(Job job) noexcept
{
	jobs.push_back(std::move(job));
}	

void Pass::Excecute(Graphics& gfx) const noexcept
{
	for (const auto& job : jobs)
	{
		job.Execute(gfx);
	}
}

void Pass::Reset() noexcept
{
	jobs.clear();
}