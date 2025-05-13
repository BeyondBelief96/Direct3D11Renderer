#include "Renderable/Plane.h"

Plane::Plane(
    Graphics& gfx,
    std::mt19937& rng,
    std::uniform_real_distribution<float>& adist,
    std::uniform_real_distribution<float>& ddist,
    std::uniform_real_distribution<float>& odist,
    std::uniform_real_distribution<float>& rdist,
    float width,
    float height,
    int divisionsX,
    int divisionsY
)
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
    // Create a plane mesh
    auto planeMesh = GeometryFactory::CreatePlane<VertexPosition>(width, height, divisionsX, divisionsY);

    // Bind vertex shader
    auto vs = AddSharedBindable<VertexShader>(gfx, "vs_plane", L"ColorIndexVS.cso");
    auto pvs = vs->GetByteCode();

    // Bind Pixel Shader
    auto ps = AddSharedBindable<PixelShader>(gfx, "ps_plane", L"ColorIndexPS.cso");

    // Bind Vertex Buffer
    std::string vbKey = "plane_vertices_" + std::to_string(divisionsX) + "_" + std::to_string(divisionsY);
    AddSharedBindable<VertexBuffer>(gfx, vbKey, planeMesh.vertices);

    // Bind Index Buffer
    std::string ibKey = "plane_indices_" + std::to_string(divisionsX) + "_" + std::to_string(divisionsY);
    AddSharedBindable<IndexBuffer>(gfx, ibKey, planeMesh.indices);

    // Create input layout
    const std::vector<D3D11_INPUT_ELEMENT_DESC> layout =
    {
        { "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
    };
    AddSharedBindable<InputLayout>(gfx, "plane_input_layout", layout, pvs);

    // Create topology
    AddSharedBindable<Topology>(gfx, "plane_topology", D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

    // Create the transform constant buffer
    AddUniqueBindable(std::make_unique<TransformConstantBuffer>(gfx, *this));

    // Create the pixel shader constant buffer with a checkerboard pattern
    struct PixelShaderConstants
    {
        struct { float r, g, b, a; } face_colors[8];
    };

    // Generate checkerboard colors
    PixelShaderConstants cb;

	// Set the colors for the checkerboard pattern
	cb.face_colors[0] = { 1.0f, 0.0f, 0.0f, 1.0f }; // Red
	cb.face_colors[1] = { 0.0f, 1.0f, 0.0f, 1.0f }; // Green
	cb.face_colors[2] = { 0.0f, 0.0f, 1.0f, 1.0f }; // Blue
	cb.face_colors[3] = { 1.0f, 1.0f, 0.0f, 1.0f }; // Yellow
	cb.face_colors[4] = { 1.0f, 0.0f, 1.0f, 1.0f }; // Magenta
	cb.face_colors[5] = { 0.0f, 1.0f, 1.0f, 1.0f }; // Cyan
	cb.face_colors[6] = { 1.0f, 1.0f, 1.0f, 1.0f }; // White
	cb.face_colors[7] = { 0.0f, 0.0f, 0.0f, 1.0f }; // Black

    // Add the pixel shader constant buffer
    AddUniqueBindable(std::make_unique<PixelConstantBuffer<PixelShaderConstants>>(gfx, cb));
}

void Plane::Update(float dt) noexcept
{
    roll += droll * dt;
    pitch += dpitch * dt;
    yaw += dyaw * dt;
    theta += dtheta * dt;
    phi += dphi * dt;
    chi += dchi * dt;
}

DirectX::XMMATRIX Plane::GetTransformXM() const noexcept
{
    return DirectX::XMMatrixRotationRollPitchYaw(pitch, yaw, roll) *
        DirectX::XMMatrixTranslation(r, 0.0f, 0.0f) *
        DirectX::XMMatrixRotationRollPitchYaw(theta, phi, chi) *
        DirectX::XMMatrixTranslation(0.0f, 0.0f, 20.0f);
}