#include "Bindable/Bindable.h"

ID3D11DeviceContext* const Bindable::GetContext(Graphics& gfx) noexcept
{
	return gfx.GetContext();
}

ID3D11Device* const Bindable::GetDevice(Graphics& gfx) noexcept
{
	return gfx.GetDevice();
}

#ifndef NDEBUG
DxgiInfoManager& Bindable::GetInfoManager(Graphics& gfx) noexcept
{
	return gfx.GetInfoManager();
}
#endif
