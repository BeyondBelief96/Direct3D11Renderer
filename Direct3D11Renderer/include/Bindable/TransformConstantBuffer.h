#pragma once
#include "ConstantBuffer.h"
#include "Renderable/Renderable.h"
#include <DirectXMath.h>
#include <memory>

class TransformConstantBuffer : public Bindable
{
public:
	enum class ShaderStage { Vertex = 1, Pixel = 2, Both = 3 };
	
	TransformConstantBuffer(Graphics& gfx, const Renderable& parent, UINT slot = 0u);
	TransformConstantBuffer(Graphics& gfx, const Renderable& parent, ShaderStage stages, 
						   UINT vertexSlot = 0u, UINT pixelSlot = 0u);
	void Bind(Graphics& gfx) noexcept override;
	
protected:
	struct TransformBuffer
	{
		DirectX::XMMATRIX modelView;
		DirectX::XMMATRIX modelViewProj;
	};
	
	void UpdateBindImpl(Graphics& gfx, const TransformBuffer& tf) noexcept;
	TransformBuffer GetTransformBuffer(Graphics& gfx) noexcept;
	
private:
	ShaderStage targetStages;
	UINT vertexSlot;
	UINT pixelSlot;
	
	static std::unique_ptr<VertexConstantBuffer<TransformBuffer>> pVertexConstantBuffer;
	static std::unique_ptr<PixelConstantBuffer<TransformBuffer>> pPixelConstantBuffer;
	
	const Renderable& parent;
};