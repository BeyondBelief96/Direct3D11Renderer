#include "Renderable/Box.h"
#include "Bindable/BindableBase.h"
#include "Bindable/BindableCache.h"
#include <memory>

Box::Box(Graphics& gfx,
	std::mt19937& rng,
	std::uniform_real_distribution<float>& adist,
	std::uniform_real_distribution<float>& ddist,
	std::uniform_real_distribution<float>& odist,
	std::uniform_real_distribution<float>& rdist)
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
	// Vertex Shader - shared
	auto vs = AddSharedBindable<VertexShader>(gfx, "box_vs", L"VertexShader.cso");
	auto pvsbc = vs->GetByteCode();

	// Pixel Shader - shared
	AddSharedBindable<PixelShader>(gfx, "box_ps", L"PixelShader.cso");

	struct Vertex
	{
		struct
		{
			float x;
			float y;
			float z;
		} pos;
	};
	const std::vector<Vertex> vertices =
	{
		{ -1.0f,-1.0f,-1.0f },
		{ 1.0f,-1.0f,-1.0f },
		{ -1.0f,1.0f,-1.0f },
		{ 1.0f,1.0f,-1.0f },
		{ -1.0f,-1.0f,1.0f },
		{ 1.0f,-1.0f,1.0f },
		{ -1.0f,1.0f,1.0f },
		{ 1.0f,1.0f,1.0f },
	};
	
	// Vertex Buffer - shared
	AddSharedBindable<VertexBuffer>(gfx, "box_vb", vertices);

	// Index buffer - shared
	const std::vector<unsigned short> indices =
	{
		0,2,1, 2,3,1,
		1,3,5, 3,7,5,
		2,6,3, 3,6,7,
		4,5,7, 4,7,6,
		0,4,2, 2,4,6,
		0,1,4, 1,5,4
	};
	AddSharedBindable<IndexBuffer>(gfx, "box_ib", indices);

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
}


void Box::Update(float dt) noexcept
{
	yaw += dyaw * dt;
	pitch += dpitch * dt;
	roll += droll * dt;
	theta += dtheta * dt;
	phi += dphi * dt;
	chi += dchi * dt;
}

DirectX::XMMATRIX Box::GetTransformXM() const noexcept
{
	return DirectX::XMMatrixRotationRollPitchYaw(pitch, yaw, roll) *
		DirectX::XMMatrixTranslation(r, 0.0f, 0.0f) *
		DirectX::XMMatrixRotationRollPitchYaw(theta, phi, chi) *
		DirectX::XMMatrixTranslation(0.0f, 0.0f, 20.0f);
}
