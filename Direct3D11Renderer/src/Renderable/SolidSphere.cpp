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

	// Vertex Shader - shared
	auto vs = AddSharedBindable<VertexShader>(gfx, "vs_solid", L"shaders/Output/SolidVS.cso");
	auto pvsbc = vs->GetByteCode();
	// Pixel Shader - shared
	AddSharedBindable<PixelShader>(gfx, "ps_solid", L"shaders/Output/SolidPS.cso");
	// Vertex Buffer - shared
	AddSharedBindable<VertexBuffer>(gfx, "sphere_vb", dynVbuf);
	// Index Buffer - shared
	AddSharedBindable<IndexBuffer>(gfx, "sphere_ib", sphereMesh.indices);
	// Input Layout - shared (from dynamic layout)
	AddSharedBindable<InputLayout>(gfx, "solid_sphere_input", dynVbuf.GetLayout().GetD3DLayout(), pvsbc);

	struct PSColorConstant
	{
		DirectX::XMVECTOR color = { 1.0f, 1.0f, 1.0f };
		float padding;
	} colorConstantBuffer;
	AddSharedBindable<PixelConstantBuffer<PSColorConstant>>(gfx, "light_color", colorConstantBuffer);
	// Topology - shared
	AddSharedBindable<Topology>(gfx, "triangle_list", D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	AddUniqueBindable(std::make_unique<TransformConstantBuffer>(gfx, *this));
}

void SolidSphere::Update(float dt) noexcept
{
}

void SolidSphere::SetPosition(DirectX::XMFLOAT3 pos) noexcept
{
	position = pos;
}

DirectX::XMMATRIX SolidSphere::GetTransformXM() const noexcept
{
	return DirectX::XMMatrixTranslation(position.x, position.y, position.z);
}
