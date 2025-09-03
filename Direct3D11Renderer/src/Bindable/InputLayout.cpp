#include "Bindable/InputLayout.h"
#include "Bindable/BindableCache.h"
#include "Exceptions/GraphicsExceptions.h"

InputLayout::InputLayout(Graphics& gfx, D3::VertexLayout layout, ID3DBlob* pVertexShaderByteCode)
	:layout(layout)
{
	DEBUGMANAGER(gfx);

	const auto d3dLayout = this->layout.GetD3DLayout();

	GFX_THROW_INFO(GetDevice(gfx)->CreateInputLayout(
		d3dLayout.data(),
		(UINT)d3dLayout.size(),
		pVertexShaderByteCode->GetBufferPointer(),
		pVertexShaderByteCode->GetBufferSize(),
		&pInputLayout
	));
}

void InputLayout::Bind(Graphics& gfx) noexcept
{
	GetContext(gfx)->IASetInputLayout(pInputLayout.Get());
}

std::string InputLayout::GetUID() const noexcept
{
	return GenerateUID(layout);
}

std::shared_ptr<InputLayout> InputLayout::Resolve(Graphics& gfx, D3::VertexLayout layout, ID3DBlob* pVertexShaderByteCode)
{
	return BindableCache::Resolve<InputLayout>(gfx, layout, pVertexShaderByteCode);
}

std::string InputLayout::GenerateUID(const D3::VertexLayout& layout, ID3DBlob* pVertexShaderByteCode)
{
	return typeid(InputLayout).name() + std::string("#") + layout.GetCode();
}
