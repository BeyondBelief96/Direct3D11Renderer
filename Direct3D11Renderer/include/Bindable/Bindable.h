#pragma once
#include "Core/Graphics.h"

class Renderable;

class Bindable
{
public:
	virtual void Bind(Graphics& gfx) noexcept = 0;
	virtual std::string GetUID() const noexcept
	{
		assert(false);
		return "";
	}
	virtual ~Bindable() = default;
	virtual void InitializeParentReference(const Renderable&) noexcept {};
protected:
	static ID3D11DeviceContext* const GetContext(Graphics& gfx) noexcept;
	static ID3D11Device* const GetDevice(Graphics& gfx) noexcept;
#ifndef NDEBUG
	static DxgiDebugManager& GetInfoManager(Graphics& gfx) noexcept(_DEBUG);
#endif
};
