#include "Bindable/Stencil.h"
#include "Bindable/BindableCache.h"

Stencil::Stencil(Graphics& gfx, Mode mode)
	: mode(mode)
{
	D3D11_DEPTH_STENCIL_DESC dsDesc = CD3D11_DEPTH_STENCIL_DESC{ CD3D11_DEFAULT{} };

	if (mode == Mode::Write)
	{
		// Write the stencil buffer with 1's wherever we draw in the render target.
		dsDesc.StencilEnable = TRUE;
		dsDesc.StencilWriteMask = 0xFF;
		dsDesc.FrontFace.StencilFunc = D3D11_COMPARISON_ALWAYS;
		dsDesc.FrontFace.StencilPassOp = D3D11_STENCIL_OP_REPLACE;
	}
	else if (mode == Mode::Mask)
	{
		// Only draw to the render target when the stencil buffer value is not 1.
		// This will allow us to draw everywhere except where we previously drew with the Write mode.
		// Giving us a masking effect.
		dsDesc.DepthEnable = FALSE;
		dsDesc.StencilEnable = TRUE;
		dsDesc.StencilReadMask = 0xFF;
		dsDesc.FrontFace.StencilFunc = D3D11_COMPARISON_NOT_EQUAL;
		dsDesc.FrontFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
		dsDesc.FrontFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
	}

	GetDevice(gfx)->CreateDepthStencilState(&dsDesc, &pDepthStencilState);
}

void Stencil::Bind(Graphics& gfx) noexcept
{
	GetContext(gfx)->OMSetDepthStencilState(pDepthStencilState.Get(), 0xFF);
}

std::shared_ptr<Stencil> Stencil::Resolve(Graphics& gfx, Mode mode)
{
	return BindableCache::Resolve<Stencil>(gfx, mode);
}

std::string Stencil::GenerateUID(Mode mode)
{
	using namespace std::string_literals;
	return typeid(Stencil).name() + "#"s + std::to_string(static_cast<int>(mode));
}

std::string Stencil::GetUID() const noexcept
{
	return GenerateUID(mode);
}
