#include "Bindable/BindableCommon.h"
#include "Exceptions/GraphicsExceptions.h"
#include <d3dcompiler.h>

VertexShader::VertexShader(Graphics& gfx, const std::string& path) : path(path)
{
	DEBUGMANAGER(gfx);

	GFX_THROW_INFO(D3DReadFileToBlob(std::wstring{ path.begin(), path.end() }.c_str(), &pByteCodeBlob));
	GFX_THROW_INFO(GetDevice(gfx)->CreateVertexShader(
		pByteCodeBlob->GetBufferPointer(),
		pByteCodeBlob->GetBufferSize(),
		nullptr,
		&pVertexShader
	));
}

void VertexShader::Bind(Graphics& gfx) noexcept
{
	GetContext(gfx)->VSSetShader(pVertexShader.Get(), nullptr, 0u);
}

ID3DBlob* VertexShader::GetByteCode() const noexcept
{
	return pByteCodeBlob.Get();
}

std::shared_ptr<VertexShader> VertexShader::Resolve(Graphics& gfx, const std::string& path)
{
	return BindableCache::Resolve<VertexShader>(gfx, path);
}

std::string VertexShader::GenerateUID(const std::string& path)
{
	return typeid(VertexShader).name() + std::string("#") + path;
}

std::string VertexShader::GetUID() const noexcept
{
	return GenerateUID(path);
}
