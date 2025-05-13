#pragma once
#include "Graphics.h"

class Bindable
{
public:
	virtual void Bind(Graphics& gfx) noexcept = 0;
	virtual ~Bindable() = default;
protected:
	static ID3D11DeviceContext* const GetContext(Graphics& gfx) noexcept;
	static ID3D11Device* const GetDevice(Graphics& gfx) noexcept;
#ifndef NDEBUG
	static DxgiInfoManager& GetInfoManager(Graphics& gfx) noexcept(_DEBUG);
#endif
};
