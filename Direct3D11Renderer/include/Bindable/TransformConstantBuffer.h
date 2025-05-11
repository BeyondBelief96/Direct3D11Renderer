#pragma once
#include "ConstantBuffer.h"
#include "Renderable/Renderable.h"
#include <DirectXMath.h>

class TransformConstantBuffer : public Bindable
{
public:
	TransformConstantBuffer(Graphics& gfx, const Renderable& parent);
	void Bind(Graphics& gfx) noexcept override;
private:
	VertexConstantBuffer<DirectX::XMMATRIX> vertexConstantBuffer;
	const Renderable& parent;
};