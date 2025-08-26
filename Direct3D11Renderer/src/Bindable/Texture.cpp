#include "Bindable/Texture.h"
#include "Exceptions/GraphicsExceptions.h"
#include "Utilities/WICFactory.h"

Texture::Texture(Graphics& gfx, const std::wstring& path)
{
	DEBUGMANAGER(gfx);

	// Load image using WIC
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

	// Get the first frame of the image
	Microsoft::WRL::ComPtr<IWICBitmapFrameDecode> pFrame;
	GFX_THROW_INFO(pDecoder->GetFrame(0, &pFrame));

	// Get image format
	WICPixelFormatGUID pixelFormat;
	GFX_THROW_INFO(pFrame->GetPixelFormat(&pixelFormat));

	// Convert format to RGBA32 if needed
	Microsoft::WRL::ComPtr<IWICFormatConverter> pConverter;
	if (pixelFormat != GUID_WICPixelFormat32bppRGBA)
	{
		GFX_THROW_INFO(WICFactory::GetFactory()->CreateFormatConverter(&pConverter));
		GFX_THROW_INFO(pConverter->Initialize(
			pFrame.Get(),
			GUID_WICPixelFormat32bppRGBA,
			WICBitmapDitherTypeNone,
			nullptr,
			0.0f,
			WICBitmapPaletteTypeCustom
		));
	}

	// Get image dimensions
	UINT width, height;
	GFX_THROW_INFO(pFrame->GetSize(&width, &height));

	// Create temporary buffer for image data
	std::vector<uint8_t> buffer(width * height * 4); // 4 byte per pixel (RGBA)

	// Copy image data to buffer
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

	// Create texture description
	D3D11_TEXTURE2D_DESC textureDesc = {};
	textureDesc.Width = width;
	textureDesc.Height = height;
	textureDesc.MipLevels = 1;
	textureDesc.ArraySize = 1;
	textureDesc.Format = DXGI_FORMAT_B8G8R8A8_UNORM;
	textureDesc.SampleDesc.Count = 1;
	textureDesc.SampleDesc.Quality = 0;
	textureDesc.Usage = D3D11_USAGE_DEFAULT;
	textureDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
	textureDesc.CPUAccessFlags = 0;
	textureDesc.MiscFlags = 0;

	// Create texture subresource data
	D3D11_SUBRESOURCE_DATA subresourceData = {};
	subresourceData.pSysMem = buffer.data();
	subresourceData.SysMemPitch = width * 4; // 4 bytes per pixel (RGBA)
	subresourceData.SysMemSlicePitch = 0;

	// Create texture
	Microsoft::WRL::ComPtr<ID3D11Texture2D> pTexture;
	GFX_THROW_INFO(GetDevice(gfx)->CreateTexture2D(
		&textureDesc,
		&subresourceData,
		pTexture.ReleaseAndGetAddressOf()
	));

	// Create shader resource view
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
	GetContext(gfx)->PSSetShaderResources(0u, 1u, pTextureView.GetAddressOf());
}
