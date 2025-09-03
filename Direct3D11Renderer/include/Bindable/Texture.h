#pragma once

#include "Bindable.h"
#include <wrl.h>
#include <string>

class Texture : public Bindable
{
public:
	Texture(Graphics& gfx, const std::wstring& path, UINT slot);
	Texture(Graphics& gfx, const std::string& path, UINT slot);
	Texture(Graphics& gfx, const char* path, UINT slot);
	void Bind(Graphics& gfx) noexcept override;
private:
	void LoadFromWideString(Graphics& gfx, const std::wstring& path);
	unsigned int slot;
	std::string path;
protected:
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> pTextureView;
};