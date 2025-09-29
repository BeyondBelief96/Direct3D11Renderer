#pragma once

#include "Bindable.h"
#include "Geometry/Vertex.h"
#include <d3d11.h>
#include <wrl.h>
#include <memory>


class InputLayout : public Bindable
{
public:
	InputLayout(Graphics& gfx, D3::VertexLayout layout, ID3DBlob* pVertexShaderByteCode);
	void Bind(Graphics& gfx) noexcept override;
	std::string GetUID() const noexcept override;
	const D3::VertexLayout GetLayout() const noexcept;

	static std::shared_ptr<InputLayout> Resolve(Graphics& gfx, D3::VertexLayout layout, ID3DBlob* pVertexShaderByteCode);
	static std::string GenerateUID(const D3::VertexLayout& layout, ID3DBlob* pVertexShaderByteCode = nullptr);
protected:
	D3::VertexLayout layout;
	Microsoft::WRL::ComPtr<ID3D11InputLayout> pInputLayout;
};