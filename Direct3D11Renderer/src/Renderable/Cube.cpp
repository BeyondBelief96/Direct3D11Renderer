#include "Renderable/Cube.h"
#include "Bindable/BindableBase.h"
#include "Bindable/BindableCache.h"
#include "Geometry/GeometryFactory.h"
#include <memory>

Cube::Cube(Graphics& gfx,
	std::mt19937& rng,
	std::uniform_real_distribution<float>& adist,
	std::uniform_real_distribution<float>& ddist,
	std::uniform_real_distribution<float>& odist,
	std::uniform_real_distribution<float>& rdist,
	std::uniform_real_distribution<float>& bdist)
	:
	r(rdist(rng)),
	droll(ddist(rng)),
	dpitch(ddist(rng)),
	dyaw(ddist(rng)),
	dphi(odist(rng)),
	dtheta(odist(rng)),
	dchi(odist(rng)),
	chi(adist(rng)),
	theta(adist(rng)),
	phi(adist(rng))
{
	auto cubeMesh = GeometryFactory::CreateCube<VertexPosition>();
	cubeMesh.SetFlatNormals();

	// Vertex Shader - shared
	auto vs = AddSharedBindable<VertexShader>(gfx, "box_vs", L"VertexShader.cso");
	auto pvsbc = vs->GetByteCode();

	// Pixel Shader - shared
	AddSharedBindable<PixelShader>(gfx, "box_ps", L"PixelShader.cso");

	// Vertex Buffer - shared
	AddSharedBindable<VertexBuffer>(gfx, "box_vb", cubeMesh.vertices);

	// Index Buffer - shared
	AddSharedBindable<IndexBuffer>(gfx, "box_ib", cubeMesh.indices);

	// Input Layout - shared
	const std::vector<D3D11_INPUT_ELEMENT_DESC> layout =
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	};
	AddSharedBindable<InputLayout>(gfx, "box_input", layout, pvsbc);

	// Color buffer - shared (all boxes use same colors)
	struct ConstantBuffer2
	{
		struct { float r, g, b, a; } face_colors[6];
	};
	const ConstantBuffer2 cb2 =
	{
		{
			{ 1.0f,0.0f,1.0f },
			{ 1.0f,0.0f,0.0f },
			{ 0.0f,1.0f,0.0f },
			{ 0.0f,0.0f,1.0f },
			{ 1.0f,1.0f,0.0f },
			{ 0.0f,1.0f,1.0f },
		}
	};
	AddSharedBindable<PixelConstantBuffer<ConstantBuffer2>>(gfx, "box_color", cb2);

	// Topology - shared
	AddSharedBindable<Topology>(gfx, "triangle_list", D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	// Transform buffer - unique per box
	AddUniqueBindable(std::make_unique<TransformConstantBuffer>(gfx, *this));

	// Apply deformation transform
	DirectX::XMStoreFloat3x3(
		&mt,
		DirectX::XMMatrixScaling(1.0f, 1.0f, bdist(rng))
	);
}


void Cube::Update(float dt) noexcept
{
	yaw += dyaw * dt;
	pitch += dpitch * dt;
	roll += droll * dt;
	theta += dtheta * dt;
	phi += dphi * dt;
	chi += dchi * dt;
}

DirectX::XMMATRIX Cube::GetTransformXM() const noexcept
{
	return DirectX::XMMatrixRotationRollPitchYaw(pitch, yaw, roll) *
		DirectX::XMMatrixTranslation(r, 0.0f, 0.0f) *
		DirectX::XMMatrixRotationRollPitchYaw(theta, phi, chi);
}
