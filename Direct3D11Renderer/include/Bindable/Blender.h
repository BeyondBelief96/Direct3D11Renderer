#pragma once
#include "Bindable.h"
#include <memory>
#include <array>

class Blender : public Bindable
{
public:
	Blender(Graphics& gfx, bool blendEnable) noexcept;
	void Bind(Graphics& gfx) noexcept override;
	static std::shared_ptr<Blender> Resolve(Graphics& gfx, bool blendEnable = true) noexcept;
	static std::string GenerateUID(bool blendEnable) noexcept;
	std::string GetUID() const noexcept override;
private:
	Microsoft::WRL::ComPtr<ID3D11BlendState> pBlender;
	bool blendEnable = false;
};