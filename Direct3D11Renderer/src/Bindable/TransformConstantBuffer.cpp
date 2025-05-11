#include "Bindable/TransformConstantBuffer.h"

TransformConstantBuffer::TransformConstantBuffer(Graphics& gfx, const Renderable& parent)
	:
	vertexConstantBuffer(gfx),
	parent(parent)
{
}

void TransformConstantBuffer::Bind(Graphics& gfx) noexcept
{
	vertexConstantBuffer.Update(gfx, DirectX::XMMatrixTranspose(
		parent.GetTransformXM() * gfx.GetProjection()));
	vertexConstantBuffer.Bind(gfx);
}
