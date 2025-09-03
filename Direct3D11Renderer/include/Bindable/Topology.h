#pragma once
#include "Bindable/BindableCommon.h"

class Topology : public Bindable
{
public:
	Topology(Graphics& gfx, D3D11_PRIMITIVE_TOPOLOGY topology) noexcept;
	void Bind(Graphics& gfx) noexcept override;
	std::string GetUID() const noexcept override;

	static std::shared_ptr<Topology> Resolve(Graphics& gfx, D3D11_PRIMITIVE_TOPOLOGY topology) noexcept;
	static std::string GenerateUID(D3D11_PRIMITIVE_TOPOLOGY topology) noexcept;
protected:
	D3D11_PRIMITIVE_TOPOLOGY topology;
};
