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
