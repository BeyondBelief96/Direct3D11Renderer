#include "Bindable/TransformConstantBuffer.h"

TransformConstantBuffer::TransformConstantBuffer(Graphics& gfx, const Renderable& parent)
	:
	parent(parent)
{
	if (!pVertexConstantBuffer)
	{
		pVertexConstantBuffer = std::make_unique<VertexConstantBuffer<TransformBuffer>>(gfx);
	}
}

void TransformConstantBuffer::Bind(Graphics& gfx) noexcept
{
    // Get the model matrix from the renderable
    DirectX::XMMATRIX model = parent.GetTransformXM();

	const TransformBuffer transformBuffer
	{
		DirectX::XMMatrixTranspose(model),
		DirectX::XMMatrixTranspose(
			model * gfx.GetViewProjection())
	};

    // Update and bind the constant buffer
    pVertexConstantBuffer->Update(gfx, transformBuffer);
    pVertexConstantBuffer->Bind(gfx);
}

std::unique_ptr<VertexConstantBuffer<TransformConstantBuffer::TransformBuffer>>
TransformConstantBuffer::pVertexConstantBuffer = nullptr;
