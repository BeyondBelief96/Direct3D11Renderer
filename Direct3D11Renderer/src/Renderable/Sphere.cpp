#include "Renderable/Sphere.h"

Sphere::Sphere(Graphics& gfx, std::mt19937& rng, std::uniform_real_distribution<float>& adist, std::uniform_real_distribution<float>& ddist, std::uniform_real_distribution<float>& odist, std::uniform_real_distribution<float>& rdist, float radius, int tessellation)
	: r(rdist(rng)), 
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
	// Create a sphere mesh
    auto sphereMesh = GeometryFactory::CreateSphere<VertexPosition>(radius, tessellation);
    
    // Bind vertex shader
    auto vs = AddSharedBindable<VertexShader>(gfx, "vs_sphere", L"ColorIndexVS.cso");
    auto pvs = vs->GetByteCode();

    // Bind Pixel Shader
	auto ps = AddSharedBindable<PixelShader>(gfx, "ps_sphere", L"ColorIndexPS.cso");

    // Bind Vertex Buffer
	AddSharedBindable<VertexBuffer>(gfx, "sphere_vertices" + tessellation, sphereMesh.vertices);

    // Bind Index Buffer
	AddSharedBindable<IndexBuffer>(gfx, "sphere_indices" + tessellation, sphereMesh.indices);

    // Create input layout
    const std::vector<D3D11_INPUT_ELEMENT_DESC> layout = 
    {
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	};
	AddSharedBindable<InputLayout>(gfx, "sphere_input_layout", layout, pvs);

    // Create topology
	AddSharedBindable<Topology>(gfx, "sphere_topology", D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

    // Create the transform constant buffer
    AddUniqueBindable(std::make_unique<TransformConstantBuffer>(gfx, *this));

	// Create the color constant buffer
    // Create the pixel shader constant buffer with colors
    struct PixelShaderConstants
    {
        struct { float r, g, b, a; } face_colors[8];
    };

    // Generate some random colors for the sphere
    std::uniform_real_distribution<float> cdist(0.0f, 1.0f);
    const PixelShaderConstants cb =
    {
        {
            { 1.0f,1.0f,1.0f },
            { 1.0f,0.0f,0.0f },
            { 0.0f,1.0f,0.0f },
            { 1.0f,1.0f,0.0f },
            { 0.0f,0.0f,1.0f },
            { 1.0f,0.0f,1.0f },
            { 0.0f,1.0f,1.0f },
            { 0.0f,0.0f,0.0f },
        }
    };

	// Add the pixel shader constant buffer
    AddUniqueBindable(std::make_unique<PixelConstantBuffer<PixelShaderConstants>>(gfx, cb));

}

void Sphere::Update(float dt) noexcept
{
    roll += droll * dt;
    pitch += dpitch * dt;
    yaw += dyaw * dt;
    theta += dtheta * dt;
    phi += dphi * dt;
    chi += dchi * dt;
}

DirectX::XMMATRIX Sphere::GetTransformXM() const noexcept
{
    return DirectX::XMMatrixRotationRollPitchYaw(pitch, yaw, roll) *
        DirectX::XMMatrixTranslation(r, 0.0f, 0.0f) *
        DirectX::XMMatrixRotationRollPitchYaw(theta, phi, chi) *
        DirectX::XMMatrixTranslation(0.0f, 0.0f, 20.0f);
}
