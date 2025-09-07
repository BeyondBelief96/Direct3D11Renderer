#pragma once

#include "BindableCommon.h"
#include <wrl.h>
#include <string>

class Texture : public Bindable
{
public:
	Texture(Graphics& gfx, const std::string& path, UINT slot);
	void Bind(Graphics& gfx) noexcept override;
	std::string GetUID() const noexcept override;

	static std::shared_ptr<Texture> Resolve(Graphics& gfx, const std::string& path, UINT slot = 0);
	static std::string GenerateUID(const std::string& path, UINT slot);
private:
	void LoadFromWideString(Graphics& gfx, const std::wstring& path);
	unsigned int slot;
	std::string path;
protected:
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> pTextureView;
};