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
	static std::unique_ptr<VertexConstantBuffer<DirectX::XMMATRIX>> pVertexConstantBuffer;
	const Renderable& parent;
};