#pragma once
#include "Bindable/Bindable.h"

class Topology : public Bindable
{
public:
	Topology(Graphics& gfx, D3D11_PRIMITIVE_TOPOLOGY topology) noexcept;
	void Bind(Graphics& gfx) noexcept override;
protected:
	D3D11_PRIMITIVE_TOPOLOGY topology;
};
