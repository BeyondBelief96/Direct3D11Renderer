#include "Bindable/Texture.h"
#include "Exceptions/GraphicsExceptions.h"
#include "Utilities/WICFactory.h"

Texture::Texture(Graphics& gfx, const std::wstring& path, unsigned int slot = 0)
{
	LoadFromWideString(gfx, path);
}

Texture::Texture(Graphics& gfx, const std::string& path, unsigned int slot = 0)
{
	std::wstring widePath;
	widePath.reserve(path.length());
	for (const auto& c : path)
	{
		widePath.push_back(static_cast<wchar_t>(c));
	}
	LoadFromWideString(gfx, widePath);
}

Texture::Texture(Graphics& gfx, const char* path, unsigned int slot = 0)
{
	std::wstring widePath;
	std::string stringPath(path);
	widePath.reserve(stringPath.length());
	for (const auto& c : stringPath)
	{
		widePath.push_back(static_cast<wchar_t>(c));
	}
	LoadFromWideString(gfx, widePath);
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

	D3D11_TEXTURE2D_DESC textureDesc = {};
	textureDesc.Width = width;
	textureDesc.Height = height;
	textureDesc.MipLevels = 1;
	textureDesc.ArraySize = 1;
	textureDesc.Format = dxgiFormat;
	textureDesc.SampleDesc.Count = 1;
	textureDesc.SampleDesc.Quality = 0;
	textureDesc.Usage = D3D11_USAGE_DEFAULT;
	textureDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
	textureDesc.CPUAccessFlags = 0;
	textureDesc.MiscFlags = 0;

	D3D11_SUBRESOURCE_DATA subresourceData = {};
	subresourceData.pSysMem = buffer.data();
	subresourceData.SysMemPitch = width * 4;
	subresourceData.SysMemSlicePitch = 0;

	Microsoft::WRL::ComPtr<ID3D11Texture2D> pTexture;
	GFX_THROW_INFO(GetDevice(gfx)->CreateTexture2D(
		&textureDesc,
		&subresourceData,
		pTexture.ReleaseAndGetAddressOf()
	));

	D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
	srvDesc.Format = textureDesc.Format;
	srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	srvDesc.Texture2D.MostDetailedMip = 0;
	srvDesc.Texture2D.MipLevels = 1;

	GFX_THROW_INFO(GetDevice(gfx)->CreateShaderResourceView(
		pTexture.Get(),
		&srvDesc,
		pTextureView.GetAddressOf()
	));
}

void Texture::Bind(Graphics& gfx) noexcept
{
	GetContext(gfx)->PSSetShaderResources(slot, 1u, pTextureView.GetAddressOf());
}
