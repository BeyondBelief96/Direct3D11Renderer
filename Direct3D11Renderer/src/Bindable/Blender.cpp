#include "Bindable/Blender.h"
#include "Bindable/BindableCache.h"

Blender::Blender(Graphics& gfx, bool blendEnable) noexcept : blendEnable(blendEnable)
{
	DEBUGMANAGER(gfx);

	D3D11_BLEND_DESC desc = {};
	auto& renderTarget = desc.RenderTarget[0];
	if (blendEnable)
	{
		renderTarget.BlendEnable = TRUE;
		renderTarget.SrcBlend = D3D11_BLEND_SRC_ALPHA;
		renderTarget.DestBlend = D3D11_BLEND_INV_SRC_ALPHA;
		renderTarget.BlendOp = D3D11_BLEND_OP_ADD;
		renderTarget.SrcBlendAlpha = D3D11_BLEND_ZERO;
		renderTarget.DestBlendAlpha = D3D11_BLEND_ZERO;
		renderTarget.BlendOpAlpha = D3D11_BLEND_OP_ADD;
		renderTarget.RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;
	}
	else
	{
		renderTarget.BlendEnable = FALSE;
		renderTarget.RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;
	}

	GFX_THROW_INFO(GetDevice(gfx)->CreateBlendState(&desc, &pBlender));
}

void Blender::Bind(Graphics& gfx) noexcept
{
	GetContext(gfx)->OMSetBlendState(pBlender.Get(), nullptr, 0xFFFFFFFFu);
}

std::shared_ptr<Blender> Blender::Resolve(Graphics& gfx, bool blendEnable) noexcept
{
	return BindableCache::Resolve<Blender>(gfx, blendEnable);
}

std::string Blender::GenerateUID(bool blendEnable) noexcept
{
	return typeid(Blender).name() + std::string("#") + (blendEnable ? "b" : "n");
}

std::string Blender::GetUID() const noexcept
{
	return GenerateUID(blendEnable);
}
