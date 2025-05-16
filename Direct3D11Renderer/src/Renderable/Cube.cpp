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
	std::uniform_real_distribution<float>& bdist,
	DirectX::XMFLOAT3 materialColor)
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
	auto cubeMesh = GeometryFactory::CreateIndependentCube<VertexPositionNormal>();

	// Vertex Shader - shared
	auto vs = AddSharedBindable<VertexShader>(gfx, "box_vs", L"PhongVS.cso");
	auto pvsbc = vs->GetByteCode();

	// Pixel Shader - shared
	AddSharedBindable<PixelShader>(gfx, "box_ps", L"PhongPS.cso");

	// Vertex Buffer - shared
	AddSharedBindable<VertexBuffer>(gfx, "box_vb", cubeMesh.vertices);

	// Index Buffer - shared
	AddSharedBindable<IndexBuffer>(gfx, "box_ib", cubeMesh.indices);

	// Input Layout - shared
	const std::vector<D3D11_INPUT_ELEMENT_DESC> layout =
	{
		{ "Position", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "Normal", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	};
	AddSharedBindable<InputLayout>(gfx, "box_input", layout, pvsbc);

	struct PSObjectConstantBuffer
	{
		alignas(16) DirectX::XMFLOAT3 materialColor = { 1.0f, 1.0f, 1.0f };  // 12 bytes + 4 bytes padding (implicit)
		float specularIntensity = 0.6f;                                      // 4 bytes
		float specularPower = 30.0f;                                         // 4 bytes
		float padding[2] = {};                                               // 8 bytes padding to make the total size a multiple of 16
	} objectConstantBuffer;

	objectConstantBuffer.materialColor = materialColor;
	AddUniqueBindable(std::make_unique<PixelConstantBuffer<PSObjectConstantBuffer>>(gfx, objectConstantBuffer, 1u));

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
