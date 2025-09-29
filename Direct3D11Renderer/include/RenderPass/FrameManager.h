#pragma once

#include <array>
#include "Core/Graphics.h"
#include "Job.h"
#include "Pass.h"

class FrameManager
{
public:
	void Accept(Job job, size_t target) noexcept;
	void Excecute(Graphics& gfx) const;
	void Reset() noexcept;
private:
	std::array<Pass, 3> passes;
};
