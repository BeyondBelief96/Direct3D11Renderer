#include "Bindable/TransformConstantBuffer.h"
#include <cassert>

TransformConstantBuffer::TransformConstantBuffer(Graphics& gfx, UINT slot)
	: targetStages(ShaderStage::Vertex), vertexSlot(slot), pixelSlot(0u)
{
	if (!pVertexConstantBuffer)
	{
		pVertexConstantBuffer = std::make_unique<VertexConstantBuffer<TransformBuffer>>(gfx, slot);
	}
}

TransformConstantBuffer::TransformConstantBuffer(Graphics& gfx, 
												 ShaderStage stages, UINT vertexSlot, UINT pixelSlot)
	: targetStages(stages), vertexSlot(vertexSlot), pixelSlot(pixelSlot)
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
		assert(parent != nullptr);
		pVertexConstantBuffer->Update(gfx, tf);
		pVertexConstantBuffer->Bind(gfx);
	}
	
	if (static_cast<int>(targetStages) & static_cast<int>(ShaderStage::Pixel))
	{
		assert(parent != nullptr);
		pPixelConstantBuffer->Update(gfx, tf);
		pPixelConstantBuffer->Bind(gfx);
	}
}

TransformConstantBuffer::TransformBuffer TransformConstantBuffer::GetTransformBuffer(Graphics & gfx) noexcept
{
	assert(parent != nullptr);
	DirectX::XMMATRIX modelView = parent->GetTransformXM() * gfx.GetView();

	const TransformBuffer transformBuffer
	{
		DirectX::XMMatrixTranspose(modelView),
		DirectX::XMMatrixTranspose(modelView * gfx.GetProjection())
	};

	return transformBuffer;
}

void TransformConstantBuffer::InitializeParentReference(const Renderable& parent) noexcept
{
	this->parent = &parent;
}

std::unique_ptr<VertexConstantBuffer<TransformConstantBuffer::TransformBuffer>>
TransformConstantBuffer::pVertexConstantBuffer = nullptr;

std::unique_ptr<PixelConstantBuffer<TransformConstantBuffer::TransformBuffer>>
TransformConstantBuffer::pPixelConstantBuffer = nullptr;
