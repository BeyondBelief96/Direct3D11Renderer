#include "Bindable/Texture.h"
#include "Exceptions/GraphicsExceptions.h"
#include "Utilities/WICFactory.h"

Texture::Texture(Graphics& gfx, const std::string& path, UINT slot, bool alphaLoaded) : slot(slot), path(path), alphaChannelLoaded(alphaLoaded)
{
	std::wstring widePath;
	widePath.reserve(path.length());
	for (const auto& c : path)
	{
		widePath.push_back(static_cast<wchar_t>(c));
	}
	LoadFromWideString(gfx, widePath);
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


void Texture::LoadFromWideString(Graphics& gfx, const std::wstring& path)
{
	DEBUGMANAGER(gfx);

	Microsoft::WRL::ComPtr<IWICBitmapDecoder> pDecoder;
	hr = WICFactory::GetFactory()->CreateDecoderFromFilename(
		path.c_str(),
		nullptr,
		GENERIC_READ,
		WICDecodeMetadataCacheOnLoad,
		&pDecoder
	);

	if (FAILED(hr))
	{
		throw GFX_EXCEPT(hr);
	}

	Microsoft::WRL::ComPtr<IWICBitmapFrameDecode> pFrame;
	GFX_THROW_INFO(pDecoder->GetFrame(0, &pFrame));

	WICPixelFormatGUID pixelFormat;
	GFX_THROW_INFO(pFrame->GetPixelFormat(&pixelFormat));

	// Check if source format has alpha channel
	bool sourceHasAlpha = (pixelFormat == GUID_WICPixelFormat32bppRGBA ||
	                      pixelFormat == GUID_WICPixelFormat32bppBGRA ||
	                      pixelFormat == GUID_WICPixelFormat32bppPRGBA ||
	                      pixelFormat == GUID_WICPixelFormat32bppPBGRA ||
	                      pixelFormat == GUID_WICPixelFormat64bppRGBA ||
	                      pixelFormat == GUID_WICPixelFormat64bppBGRA ||
	                      pixelFormat == GUID_WICPixelFormat64bppPRGBA ||
	                      pixelFormat == GUID_WICPixelFormat64bppPBGRA ||
	                      pixelFormat == GUID_WICPixelFormat128bppRGBAFloat ||
	                      pixelFormat == GUID_WICPixelFormat128bppPRGBAFloat);

	// Determine the best target format based on source
	WICPixelFormatGUID targetFormat;
	DXGI_FORMAT dxgiFormat;
	
	// Check if source is already in a format we can use directly
	if (pixelFormat == GUID_WICPixelFormat32bppRGBA)
	{
		targetFormat = GUID_WICPixelFormat32bppRGBA;
		dxgiFormat = DXGI_FORMAT_R8G8B8A8_UNORM;
	}
	else if (pixelFormat == GUID_WICPixelFormat32bppBGRA)
	{
		targetFormat = GUID_WICPixelFormat32bppBGRA;
		dxgiFormat = DXGI_FORMAT_B8G8R8A8_UNORM;
	}
	else
	{
		// Default to RGBA for unknown formats
		targetFormat = GUID_WICPixelFormat32bppRGBA;
		dxgiFormat = DXGI_FORMAT_R8G8B8A8_UNORM;
	}

	Microsoft::WRL::ComPtr<IWICFormatConverter> pConverter;
	if (pixelFormat != targetFormat)
	{
		GFX_THROW_INFO(WICFactory::GetFactory()->CreateFormatConverter(&pConverter));
		GFX_THROW_INFO(pConverter->Initialize(
			pFrame.Get(),
			targetFormat,
			WICBitmapDitherTypeNone,
			nullptr,
			0.0f,
			WICBitmapPaletteTypeCustom
		));
	}

	UINT width, height;
	GFX_THROW_INFO(pFrame->GetSize(&width, &height));

	std::vector<uint8_t> buffer(width * height * 4);

	if (pConverter)
	{
		GFX_THROW_INFO(pConverter->CopyPixels(
			nullptr,
			width * 4,
			static_cast<UINT>(buffer.size()),
			buffer.data()
		));
	}
	else
	{
		GFX_THROW_INFO(pFrame->CopyPixels(
			nullptr,
			width * 4,
			static_cast<UINT>(buffer.size()),
			buffer.data()
		));
	}

	// Detect if alpha channel is actually used (contains non-255 values)
	if (sourceHasAlpha && !alphaChannelLoaded)
	{
		// Check if any alpha values are not fully opaque (255)
		for (size_t i = 3; i < buffer.size(); i += 4) // Alpha is every 4th byte (RGBA/BGRA)
		{
			if (buffer[i] != 255)
			{
				alphaChannelLoaded = true;
				break;
			}
		}
	}

	D3D11_TEXTURE2D_DESC textureDesc = {};
	textureDesc.Width = width;
	textureDesc.Height = height;
	textureDesc.MipLevels = 0;
	textureDesc.ArraySize = 1;
	textureDesc.Format = dxgiFormat;
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

	// Write texture data into the top mip level.
	GetContext(gfx)->UpdateSubresource(pTexture.Get(), 0u, nullptr, buffer.data(), width * 4, 0u);

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

	GetContext(gfx)->GenerateMips(pTextureView.Get());
}

void Texture::Bind(Graphics& gfx) noexcept
{
	GetContext(gfx)->PSSetShaderResources(slot, 1u, pTextureView.GetAddressOf());
}

