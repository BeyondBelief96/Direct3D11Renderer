#pragma once

#include "Bindable.h"
#include <wrl.h>
#include <string>

class Texture : public Bindable
{
public:
	Texture(Graphics& gfx, const std::wstring& path, unsigned int slot);
	Texture(Graphics& gfx, const std::string& path, unsigned int slot);
	Texture(Graphics& gfx, const char* path, unsigned int slot);
	void Bind(Graphics& gfx) noexcept override;
private:
	void LoadFromWideString(Graphics& gfx, const std::wstring& path);
	unsigned int slot;
protected:
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> pTextureView;
};