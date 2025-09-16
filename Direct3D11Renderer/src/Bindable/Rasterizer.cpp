#include "Bindable/Rasterizer.h"
#include "Bindable/BindableCache.h"

Rasterizer::Rasterizer(Graphics& gfx, bool twoSided) : twoSided(twoSided)
{
	DEBUGMANAGER(gfx);
	D3D11_RASTERIZER_DESC rasterizerDesc = CD3D11_RASTERIZER_DESC(CD3D11_DEFAULT{});
	rasterizerDesc.CullMode = twoSided ? D3D11_CULL_NONE : D3D11_CULL_BACK;
	GFX_THROW_INFO(GetDevice(gfx)->CreateRasterizerState(&rasterizerDesc, &pRasterizer));
}

void Rasterizer::Bind(Graphics& gfx) noexcept
{
	GetContext(gfx)->RSSetState(pRasterizer.Get());
}

std::shared_ptr<Rasterizer> Rasterizer::Resolve(Graphics& gfx, bool twoSided)
{
	return BindableCache::Resolve<Rasterizer>(gfx, twoSided);
}

std::string Rasterizer::GenerateUID(bool twoSided)
{
	using namespace std::string_literals;
	return typeid(Rasterizer).name() + "#"s + (twoSided ? "2s" : "1s");
}

std::string Rasterizer::GetUID() const noexcept
{
	return GenerateUID(twoSided);
}
