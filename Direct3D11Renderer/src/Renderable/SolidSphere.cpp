#include "Renderable/SolidSphere.h"
#include "Geometry/GeometryFactory.h"
#include "Bindable/BindableCommon.h"
#include "Geometry/Vertex.h"

SolidSphere::SolidSphere(Graphics& gfx, float radius)
{
	auto sphereMesh = GeometryFactory::CreateSphere<VertexPositionNormal>(radius, 10, 10);
	sphereMesh.Transform(DirectX::XMMatrixScaling(radius, radius, radius));
	const auto geometryTag = "$sphere." + std::to_string(radius);

	D3::VertexLayout dynLayout;
	dynLayout.Append(D3::VertexLayout::Position3D);
	dynLayout.Append(D3::VertexLayout::Normal);
	D3::VertexBuffer dynVbuf(std::move(dynLayout));
	for (const auto& v : sphereMesh.vertices)
	{
		dynVbuf.EmplaceBack(v.position, v.normal);
	}

	pVertices = VertexBuffer::Resolve(gfx, geometryTag, dynVbuf);
	pIndices = IndexBuffer::Resolve(gfx, geometryTag, sphereMesh.indices);
	pTopology = Topology::Resolve(gfx, D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	Technique solid;
	Step only(0);

	auto vs = VertexShader::Resolve(gfx, "shaders\\Output\\SolidColor_VS.cso");
	auto pvsbc = vs->GetByteCode();
	only.AddBindable(std::move(vs));
	only.AddBindable(PixelShader::Resolve(gfx, "shaders\\Output\\SolidColor_PS.cso"));
	struct PSColorConstant
	{
		DirectX::XMFLOAT3 color = { 1.0f, 1.0f, 1.0f };
		float padding; // Padding to ensure 16-byte alignment
	} colorConst;
	only.AddBindable(PixelConstantBuffer<PSColorConstant>::Resolve(gfx, colorConst, 1u));
	only.AddBindable(InputLayout::Resolve(gfx, dynVbuf.GetLayout(), pvsbc));
	only.AddBindable(std::make_shared<TransformConstantBuffer>(gfx));
	only.AddBindable(Rasterizer::Resolve(gfx, false));

	solid.AddStep(only);
	AddTechnique(std::move(solid));
}

void SolidSphere::SetPosition(DirectX::XMFLOAT3 pos) noexcept
{
	position = pos;
}

DirectX::XMMATRIX SolidSphere::GetTransformXM() const noexcept
{
	return DirectX::XMMatrixTranslation(position.x, position.y, position.z);
}
