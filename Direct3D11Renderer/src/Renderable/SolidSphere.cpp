#include "Renderable/SolidSphere.h"
#include "Geometry/GeometryFactory.h"
#include "Bindable/BindableCommon.h"
#include "Geometry/Vertex.h"

SolidSphere::SolidSphere(Graphics& gfx, float radius)
{
	auto sphereMesh = GeometryFactory::CreateSphere<VertexPosition>(radius, 10, 10);

	D3::VertexLayout dynLayout;
	dynLayout.Append(D3::VertexLayout::Position3D);
	D3::VertexBuffer dynVbuf(std::move(dynLayout));
	for (const auto& v : sphereMesh.vertices)
	{
		dynVbuf.EmplaceBack(v.position);
	}

	// Vertex Shader
	auto vs = std::make_shared<VertexShader>(gfx, "shaders/Output/SolidVS.cso");
	auto pvsbc = vs->GetByteCode();
	AddBindable(vs);
	
	// Pixel Shader
	AddBindable(std::make_shared<PixelShader>(gfx, "shaders/Output/SolidPS.cso"));
	
	// Vertex Buffer
	AddBindable(std::make_shared<VertexBuffer>(gfx, dynVbuf));
	
	// Index Buffer
	AddBindable(std::make_shared<IndexBuffer>(gfx, sphereMesh.indices));
	
	// Input Layout
	AddBindable(std::make_shared<InputLayout>(gfx, dynVbuf.GetLayout().GetD3DLayout(), pvsbc));

	struct PSColorConstant
	{
		DirectX::XMVECTOR color = { 1.0f, 1.0f, 1.0f };
		float padding;
	} colorConstantBuffer;
	AddBindable(std::make_shared<PixelConstantBuffer<PSColorConstant>>(gfx, colorConstantBuffer));
	
	// Topology
	AddBindable(std::make_shared<Topology>(gfx, D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST));

	AddBindable(std::make_shared<TransformConstantBuffer>(gfx, *this));
}

void SolidSphere::SetPosition(DirectX::XMFLOAT3 pos) noexcept
{
	position = pos;
}

DirectX::XMMATRIX SolidSphere::GetTransformXM() const noexcept
{
	return DirectX::XMMatrixTranslation(position.x, position.y, position.z);
}
