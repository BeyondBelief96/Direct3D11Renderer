#include "Bindable/TransformConstantBuffer.h"

TransformConstantBuffer::TransformConstantBuffer(Graphics& gfx, const Renderable& parent, UINT slot)
	:
	parent(parent)
{
	if (!pVertexConstantBuffer)
	{
		pVertexConstantBuffer = std::make_unique<VertexConstantBuffer<TransformBuffer>>(gfx, slot);
	}
}

void TransformConstantBuffer::Bind(Graphics& gfx) noexcept
{
	UpdateBindImpl(gfx, GetTransformBuffer(gfx));
}

void TransformConstantBuffer::UpdateBindImpl(Graphics& gfx, const TransformBuffer& tf) noexcept
{
	pVertexConstantBuffer->Update(gfx, tf);
	pVertexConstantBuffer->Bind(gfx);
}

TransformConstantBuffer::TransformBuffer TransformConstantBuffer::GetTransformBuffer(Graphics & gfx) noexcept
{
	DirectX::XMMATRIX modelView = parent.GetTransformXM() * gfx.GetView();

	const TransformBuffer transformBuffer
	{
		DirectX::XMMatrixTranspose(modelView),
		DirectX::XMMatrixTranspose(
			modelView * gfx.GetProjection())
	};

	return transformBuffer;
}

std::unique_ptr<VertexConstantBuffer<TransformConstantBuffer::TransformBuffer>>
TransformConstantBuffer::pVertexConstantBuffer = nullptr;
