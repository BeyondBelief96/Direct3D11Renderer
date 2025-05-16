#include "SolidSphere.h"
#include "GeometryFactory.h"
#include "BindableBase.h"

SolidSphere::SolidSphere(Graphics& gfx, float radius)
{
	auto sphereMesh = GeometryFactory::CreateSphere<VertexPosition>(radius, 10, 10);
	// Vertex Shader - shared
	auto vs = AddSharedBindable<VertexShader>(gfx, "sphere_vs", L"SolidVS.cso");
	auto pvsbc = vs->GetByteCode();
	// Pixel Shader - shared
	AddSharedBindable<PixelShader>(gfx, "sphere_ps", L"SolidPS.cso");
	// Vertex Buffer - shared
	AddSharedBindable<VertexBuffer>(gfx, "sphere_vb", sphereMesh.vertices);
	// Index Buffer - shared
	AddSharedBindable<IndexBuffer>(gfx, "sphere_ib", sphereMesh.indices);
	// Input Layout - shared
	const std::vector<D3D11_INPUT_ELEMENT_DESC> layout =
	{
		{ "Position", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	};
	AddSharedBindable<InputLayout>(gfx, "solid_sphere_input", layout, pvsbc);

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
