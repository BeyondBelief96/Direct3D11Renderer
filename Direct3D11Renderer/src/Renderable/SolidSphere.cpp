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
	auto vs = VertexShader::Resolve(gfx, "shaders/Output/PointLightIndicator_VS.cso");
	auto pvsbc = static_cast<VertexShader*>(vs.get())->GetByteCode();
	AddBindable(vs);
	
	// Pixel Shader
	AddBindable(PixelShader::Resolve(gfx, "shaders/Output/PointLightIndicator_PS.cso"));
	
	// Vertex Buffer
	AddBindable(VertexBuffer::Resolve(gfx, "sphere", dynVbuf));
	
	// Index Buffer
	AddBindable(IndexBuffer::Resolve(gfx, "sphere", sphereMesh.indices));
	
	// Input Layout
	AddBindable(InputLayout::Resolve(gfx, dynVbuf.GetLayout(), pvsbc));

	struct PSColorConstant
	{
		DirectX::XMVECTOR color = { 1.0f, 1.0f, 1.0f };
		float padding;
	} colorConstantBuffer;
	AddBindable(PixelConstantBuffer<PSColorConstant>::Resolve(gfx, colorConstantBuffer, 1u));
	
	// Topology
	AddBindable(Topology::Resolve(gfx, D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST));

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
