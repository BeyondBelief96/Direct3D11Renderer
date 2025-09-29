#pragma once

#include "Core/Graphics.h"
#include "Job.h"
#include <vector>

class Pass
{
public:
	void Accept(Job job) noexcept;
	void Excecute(Graphics& gfx) const noexcept;
	void Reset() noexcept;
private:
	std::vector<Job> jobs;
};