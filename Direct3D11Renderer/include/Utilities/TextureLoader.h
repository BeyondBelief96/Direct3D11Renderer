#pragma once

#include <string>
#include <vector>
#include <cstdint>
#include <dxgiformat.h>

struct TextureData
{
    std::vector<uint8_t> pixels;
    uint32_t width;
    uint32_t height;
    bool hasAlpha;
    DXGI_FORMAT format;
};

class ITextureLoader
{
public:
    virtual ~ITextureLoader() = default;
    virtual TextureData LoadTexture(const std::string& filePath) = 0;
};

class DirectXTexLoader : public ITextureLoader
{
public:
    TextureData LoadTexture(const std::string& filePath) override;
private:
    std::wstring ConvertToWideString(const std::string& str);
};