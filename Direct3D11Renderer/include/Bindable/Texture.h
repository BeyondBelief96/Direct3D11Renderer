#pragma once

#include "Bindable.h"
#include <wrl.h>
#include <string>

class Texture : public Bindable
{
public:
	Texture(Graphics& gfx, const std::wstring& path);
	Texture(Graphics& gfx, const std::string& path);
	Texture(Graphics& gfx, const char* path);
	void Bind(Graphics& gfx) noexcept override;
private:
	void LoadFromWideString(Graphics& gfx, const std::wstring& path);
protected:
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> pTextureView;
};