#include "D3Timer.h"

using namespace std::chrono;

D3Timer::D3Timer() noexcept
{
	last = steady_clock::now();
}

float D3Timer::Mark() noexcept
{
	const auto old = last;
	last = steady_clock::now();
	const duration<float> frameTime = last - old;
	return frameTime.count();
}

float D3Timer::Peek() const noexcept
{
	return duration<float>(steady_clock::now() - last).count();
}