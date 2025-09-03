#include "Bindable/Topology.h"

Topology::Topology(Graphics& gfx, D3D11_PRIMITIVE_TOPOLOGY topology) noexcept
	:
	topology(topology)
{
}

void Topology::Bind(Graphics& gfx) noexcept
{
	GetContext(gfx)->IASetPrimitiveTopology(topology);
}

std::string Topology::GetUID() const noexcept
{
	return GenerateUID(topology);
}

std::shared_ptr<Topology> Topology::Resolve(Graphics& gfx, D3D11_PRIMITIVE_TOPOLOGY topology) noexcept
{
	return BindableCache::Resolve<Topology>(gfx, topology);
}

std::string Topology::GenerateUID(D3D11_PRIMITIVE_TOPOLOGY topology) noexcept
{
	return typeid(Topology).name() + std::string("#") + std::to_string(topology);
}
