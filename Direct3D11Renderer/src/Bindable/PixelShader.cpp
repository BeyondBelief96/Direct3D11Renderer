#include "Bindable/PixelShader.h"
#include "Bindable/BindableCache.h"
#include "Exceptions/GraphicsExceptions.h"
#include <d3dcompiler.h>

PixelShader::PixelShader(Graphics& gfx, const std::string& path) : path(path)
{
	DEBUGMANAGER(gfx);

	Microsoft::WRL::ComPtr<ID3DBlob> pBlob;
	GFX_THROW_INFO(D3DReadFileToBlob(std::wstring{path.begin(), path.end()}.c_str(), &pBlob));
	GFX_THROW_INFO(GetDevice(gfx)->CreatePixelShader(pBlob->GetBufferPointer(), pBlob->GetBufferSize(), nullptr, &pPixelShader));
}

void PixelShader::Bind(Graphics& gfx) noexcept
{
	GetContext(gfx)->PSSetShader(pPixelShader.Get(), nullptr, 0u);
}

std::shared_ptr<Bindable> PixelShader::Resolve(Graphics& gfx, const std::string& path)
{
	return BindableCache::Resolve<PixelShader>(gfx, path);
}

std::string PixelShader::GenerateUID(const std::string& path)
{
	return typeid(PixelShader).name() + std::string("#") + path;
}

std::string PixelShader::GetUID() const noexcept
{
	return GenerateUID(path);
}