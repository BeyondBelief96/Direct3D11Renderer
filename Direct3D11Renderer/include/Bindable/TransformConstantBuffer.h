#pragma once
#include "ConstantBuffer.h"
#include "Renderable/Renderable.h"
#include <DirectXMath.h>

class TransformConstantBuffer : public Bindable
{
public:
	TransformConstantBuffer(Graphics& gfx, const Renderable& parent, UINT slot = 0u);
	void Bind(Graphics& gfx) noexcept override;
protected:
	// Transform buffer
	struct TransformBuffer
	{
		DirectX::XMMATRIX modelViewProj;
		DirectX::XMMATRIX model;
	};
	void UpdateBindImpl(Graphics& gfx, const TransformBuffer& tf) noexcept;
	TransformBuffer GetTransformBuffer(Graphics& gfx) noexcept;
private:
	static std::unique_ptr<VertexConstantBuffer<TransformBuffer>> pVertexConstantBuffer;
	const Renderable& parent;
};