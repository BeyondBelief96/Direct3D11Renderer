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
    DirectX::XMMATRIX modelView = parent.GetTransformXM() * gfx.GetView();

	const TransformBuffer transformBuffer
	{
		DirectX::XMMatrixTranspose(modelView),
		DirectX::XMMatrixTranspose(
			modelView * gfx.GetProjection())
	};

    // Update and bind the constant buffer
    pVertexConstantBuffer->Update(gfx, transformBuffer);
    pVertexConstantBuffer->Bind(gfx);
}

std::unique_ptr<VertexConstantBuffer<TransformConstantBuffer::TransformBuffer>>
TransformConstantBuffer::pVertexConstantBuffer = nullptr;
