#pragma once

class Job
{
public:
	Job(const class Renderable* pRenderable, const class Step* pStep);
	void Execute(class Graphics& gfx) const noexcept;
private:
	const class Renderable* pRenderable;
	const class Step* pStep;

};