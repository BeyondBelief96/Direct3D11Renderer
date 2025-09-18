#include "Bindable/Texture.h"
#include "Exceptions/GraphicsExceptions.h"
#include "Utilities/TextureLoader.h"

Texture::Texture(Graphics& gfx, const std::string& path, UINT slot, bool alphaLoaded)
	: slot(slot), path(path), alphaChannelLoaded(alphaLoaded), textureLoader(std::make_unique<DirectXTexLoader>())
{
	LoadFromFile(gfx, path);
}

std::shared_ptr<Texture> Texture::Resolve(Graphics& gfx, const std::string& path, UINT slot)
{
	return BindableCache::Resolve<Texture>(gfx, path, slot);
}

std::string Texture::GenerateUID(const std::string& path, UINT slot)
{
	return typeid(Texture).name() + std::string("#") + path + "#" + std::to_string(slot);
}

bool Texture::AlphaChannelLoaded() const noexcept
{
	return alphaChannelLoaded;
}

std::string Texture::GetUID() const noexcept
{
	return GenerateUID(path, slot);
}


void Texture::LoadFromFile(Graphics& gfx, const std::string& path)
{
	DEBUGMANAGER(gfx);

	// Load texture data using the abstraction
	TextureData textureData = textureLoader->LoadTexture(path);

	// Update alpha channel status if not explicitly set in constructor
	if (!alphaChannelLoaded)
	{
		alphaChannelLoaded = textureData.hasAlpha;
	}

	// Create D3D11 texture descriptor using the format from loaded texture data
	D3D11_TEXTURE2D_DESC textureDesc = {};
	textureDesc.Width = textureData.width;
	textureDesc.Height = textureData.height;
	textureDesc.MipLevels = 0;
	textureDesc.ArraySize = 1;
	textureDesc.Format = textureData.format;
	textureDesc.SampleDesc.Count = 1;
	textureDesc.SampleDesc.Quality = 0;
	textureDesc.Usage = D3D11_USAGE_DEFAULT;
	textureDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_RENDER_TARGET;
	textureDesc.CPUAccessFlags = 0;
	textureDesc.MiscFlags = D3D11_RESOURCE_MISC_GENERATE_MIPS;

	Microsoft::WRL::ComPtr<ID3D11Texture2D> pTexture;
	GFX_THROW_INFO(GetDevice(gfx)->CreateTexture2D(
		&textureDesc,
		nullptr,
		pTexture.ReleaseAndGetAddressOf()
	));

	// Write texture data into the top mip level
	// Both RGBA and BGRA formats use 4 bytes per pixel
	uint32_t bytesPerPixel = 4;
	GetContext(gfx)->UpdateSubresource(
		pTexture.Get(),
		0u,
		nullptr,
		textureData.pixels.data(),
		textureData.width * bytesPerPixel,
		0u
	);

	// Create shader resource view
	D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
	srvDesc.Format = textureDesc.Format;
	srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	srvDesc.Texture2D.MostDetailedMip = 0;
	srvDesc.Texture2D.MipLevels = -1;

	GFX_THROW_INFO(GetDevice(gfx)->CreateShaderResourceView(
		pTexture.Get(),
		&srvDesc,
		pTextureView.GetAddressOf()
	));

	// Generate mipmaps
	GetContext(gfx)->GenerateMips(pTextureView.Get());
}

void Texture::Bind(Graphics& gfx) noexcept
{
	GetContext(gfx)->PSSetShaderResources(slot, 1u, pTextureView.GetAddressOf());
}

