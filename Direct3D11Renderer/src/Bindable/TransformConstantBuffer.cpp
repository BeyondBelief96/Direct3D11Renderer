#include "Bindable/TransformConstantBuffer.h"

TransformConstantBuffer::TransformConstantBuffer(Graphics& gfx, const Renderable& parent, UINT slot)
	: parent(parent), targetStages(ShaderStage::Vertex), vertexSlot(slot), pixelSlot(0u)
{
	if (!pVertexConstantBuffer)
	{
		pVertexConstantBuffer = std::make_unique<VertexConstantBuffer<TransformBuffer>>(gfx, slot);
	}
}

TransformConstantBuffer::TransformConstantBuffer(Graphics& gfx, const Renderable& parent, 
												 ShaderStage stages, UINT vertexSlot, UINT pixelSlot)
	: parent(parent), targetStages(stages), vertexSlot(vertexSlot), pixelSlot(pixelSlot)
{
	if ((static_cast<int>(stages) & static_cast<int>(ShaderStage::Vertex)) && !pVertexConstantBuffer)
	{
		pVertexConstantBuffer = std::make_unique<VertexConstantBuffer<TransformBuffer>>(gfx, vertexSlot);
	}
	
	if ((static_cast<int>(stages) & static_cast<int>(ShaderStage::Pixel)) && !pPixelConstantBuffer)
	{
		pPixelConstantBuffer = std::make_unique<PixelConstantBuffer<TransformBuffer>>(gfx, pixelSlot);
	}
}

void TransformConstantBuffer::Bind(Graphics& gfx) noexcept
{
	UpdateBindImpl(gfx, GetTransformBuffer(gfx));
}

void TransformConstantBuffer::UpdateBindImpl(Graphics& gfx, const TransformBuffer& tf) noexcept
{
	if (static_cast<int>(targetStages) & static_cast<int>(ShaderStage::Vertex))
	{
		pVertexConstantBuffer->Update(gfx, tf);
		pVertexConstantBuffer->Bind(gfx);
	}
	
	if (static_cast<int>(targetStages) & static_cast<int>(ShaderStage::Pixel))
	{
		pPixelConstantBuffer->Update(gfx, tf);
		pPixelConstantBuffer->Bind(gfx);
	}
}

TransformConstantBuffer::TransformBuffer TransformConstantBuffer::GetTransformBuffer(Graphics & gfx) noexcept
{
	DirectX::XMMATRIX modelView = parent.GetTransformXM() * gfx.GetView();

	const TransformBuffer transformBuffer
	{
		DirectX::XMMatrixTranspose(modelView),
		DirectX::XMMatrixTranspose(modelView * gfx.GetProjection())
	};

	return transformBuffer;
}

std::unique_ptr<VertexConstantBuffer<TransformConstantBuffer::TransformBuffer>>
TransformConstantBuffer::pVertexConstantBuffer = nullptr;

std::unique_ptr<PixelConstantBuffer<TransformConstantBuffer::TransformBuffer>>
TransformConstantBuffer::pPixelConstantBuffer = nullptr;
