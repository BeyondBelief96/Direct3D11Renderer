#pragma once

#include "Bindable.h"
#include <wrl.h>

class Texture : public Bindable
{
public:
	Texture(Graphics& gfx, const std::wstring& path);
	void Bind(Graphics& gfx) noexcept override;
protected:
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> pTextureView;
};
