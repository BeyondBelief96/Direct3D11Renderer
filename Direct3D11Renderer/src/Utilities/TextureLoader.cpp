#include "Utilities/TextureLoader.h"
#include "Exceptions/GraphicsExceptions.h"
#include <DirectXTex.h>
#include <stdexcept>

TextureData DirectXTexLoader::LoadTexture(const std::string& filePath)
{
    std::wstring wideFilePath = ConvertToWideString(filePath);

    DirectX::ScratchImage scratch;
    HRESULT hr = DirectX::LoadFromWICFile(
        wideFilePath.c_str(),
        DirectX::WIC_FLAGS_NONE,
        nullptr,
        scratch
    );

    if (FAILED(hr))
    {
        throw std::runtime_error("Failed to load texture: " + filePath);
    }

    // Determine optimal target format based on source format
    DXGI_FORMAT sourceFormat = scratch.GetImage(0, 0, 0)->format;
    DXGI_FORMAT targetFormat;

    // Choose format intelligently like the original implementation
    if (sourceFormat == DXGI_FORMAT_R8G8B8A8_UNORM ||
        sourceFormat == DXGI_FORMAT_R8G8B8A8_UNORM_SRGB)
    {
        targetFormat = DXGI_FORMAT_R8G8B8A8_UNORM;
    }
    else if (sourceFormat == DXGI_FORMAT_B8G8R8A8_UNORM ||
             sourceFormat == DXGI_FORMAT_B8G8R8A8_UNORM_SRGB)
    {
        targetFormat = DXGI_FORMAT_B8G8R8A8_UNORM;
    }
    else
    {
        // Default to RGBA for unknown formats
        targetFormat = DXGI_FORMAT_R8G8B8A8_UNORM;
    }

    DirectX::ScratchImage converted;
    if (sourceFormat != targetFormat)
    {
        hr = DirectX::Convert(
            *scratch.GetImage(0, 0, 0),
            targetFormat,
            DirectX::TEX_FILTER_DEFAULT,
            DirectX::TEX_THRESHOLD_DEFAULT,
            converted
        );

        if (FAILED(hr))
        {
            throw std::runtime_error("Failed to convert texture format: " + filePath);
        }
    }
    else
    {
        // If already in correct format, move the original
        converted = std::move(scratch);
    }

    const DirectX::Image* image = converted.GetImage(0, 0, 0);

    TextureData data;
    data.width = static_cast<uint32_t>(image->width);
    data.height = static_cast<uint32_t>(image->height);
    data.format = targetFormat;

    // Calculate bytes per pixel based on format (both RGBA and BGRA are 4 bytes per pixel)
    size_t bytesPerPixel = 4;
    size_t pixelDataSize = image->width * image->height * bytesPerPixel;
    data.pixels.resize(pixelDataSize);

    // Copy row by row to handle potential row pitch differences
    const uint8_t* srcPtr = image->pixels;
    uint8_t* dstPtr = data.pixels.data();
    size_t rowSizeBytes = image->width * bytesPerPixel;

    for (size_t row = 0; row < image->height; ++row)
    {
        std::memcpy(dstPtr, srcPtr, rowSizeBytes);
        srcPtr += image->rowPitch;
        dstPtr += rowSizeBytes;
    }

    // Check for alpha channel usage (non-255 values)
    data.hasAlpha = !converted.IsAlphaAllOpaque();

    return data;
}

std::wstring DirectXTexLoader::ConvertToWideString(const std::string& str)
{
    std::wstring wideStr;
    wideStr.reserve(str.length());
    for (char c : str)
    {
        wideStr.push_back(static_cast<wchar_t>(c));
    }
    return wideStr;
}