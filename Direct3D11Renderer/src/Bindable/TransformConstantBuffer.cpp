#include "Bindable/TransformConstantBuffer.h"

TransformConstantBuffer::TransformConstantBuffer(Graphics& gfx, const Renderable& parent)
	:
	parent(parent)
{
	if (!pVertexConstantBuffer)
	{
		pVertexConstantBuffer = std::make_unique<VertexConstantBuffer<DirectX::XMMATRIX>>(gfx);
	}
}

void TransformConstantBuffer::Bind(Graphics& gfx) noexcept
{
    // Get the model matrix from the renderable
    DirectX::XMMATRIX model = parent.GetTransformXM();

    // Get the combined view-projection matrix (just one matrix multiply)
    DirectX::XMMATRIX viewProj = gfx.GetViewProjection();

    // Combine with model matrix
    DirectX::XMMATRIX modelViewProj = model * viewProj;

	// Transpose for shader consumption since HLSL uses column-major order while DirectX uses row-major order
    DirectX::XMMATRIX transposed = DirectX::XMMatrixTranspose(modelViewProj);

    // Update and bind the constant buffer
    pVertexConstantBuffer->Update(gfx, transposed);
    pVertexConstantBuffer->Bind(gfx);
}

std::unique_ptr<VertexConstantBuffer<DirectX::XMMATRIX>> TransformConstantBuffer::pVertexConstantBuffer = nullptr;
