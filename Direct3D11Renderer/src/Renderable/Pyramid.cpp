#include "Renderable/Pyramid.h"

Pyramid::Pyramid(
    Graphics& gfx,
    std::mt19937& rng,
    std::uniform_real_distribution<float>& adist,
    std::uniform_real_distribution<float>& ddist,
    std::uniform_real_distribution<float>& odist,
    std::uniform_real_distribution<float>& rdist,
    float radius,
    float height,
    int sides
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
    // Create a prism mesh (we'll use this for our pyramid by transforming it)
    auto prismMesh = GeometryFactory::CreatePrism<VertexPosition>(radius, height, sides);

    // Transform the prism into a pyramid by adjusting the vertices
    // We'll set the top cap vertices to a single point
    const float halfHeight = height / 2.0f;

    // Find all top cap vertices (they have z = halfHeight)
    for (auto& vertex : prismMesh.vertices)
    {
        if (vertex.position.z > halfHeight - 0.01f &&
            !(vertex.position.x == 0.0f && vertex.position.y == 0.0f))
        {
            // Set all top perimeter vertices to the center point
            vertex.position = { 0.0f, 0.0f, halfHeight };
        }
    }

    // Bind vertex shader
    auto vs = AddSharedBindable<VertexShader>(gfx, "vs_pyramid", L"ColorIndexVS.cso");
    auto pvs = vs->GetByteCode();

    // Bind Pixel Shader
    auto ps = AddSharedBindable<PixelShader>(gfx, "ps_pyramid", L"ColorIndexPS.cso");

    // Bind Vertex Buffer
    std::string vbKey = "pyramid_vertices_" + std::to_string(sides) + "_" + std::to_string(radius);
    AddSharedBindable<VertexBuffer>(gfx, vbKey, prismMesh.vertices);

    // Bind Index Buffer
    std::string ibKey = "pyramid_indices_" + std::to_string(sides);
    AddSharedBindable<IndexBuffer>(gfx, ibKey, prismMesh.indices);

    // Create input layout
    const std::vector<D3D11_INPUT_ELEMENT_DESC> layout =
    {
        { "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
    };
    AddSharedBindable<InputLayout>(gfx, "pyramid_input_layout", layout, pvs);

    // Create topology
    AddSharedBindable<Topology>(gfx, "pyramid_topology", D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

    // Create the transform constant buffer
    AddUniqueBindable(std::make_unique<TransformConstantBuffer>(gfx, *this));

    // Create the pixel shader constant buffer with colors
    struct PixelShaderConstants
    {
        struct { float r, g, b, a; } face_colors[8];
    };

    // Generate colors for the pyramid
    std::uniform_real_distribution<float> cdist(0.0f, 1.0f);
    PixelShaderConstants cb;
    for (int i = 0; i < 8; i++)
    {
        cb.face_colors[i] = { cdist(rng), cdist(rng), cdist(rng), 1.0f };
    }

    // Add the pixel shader constant buffer
    AddUniqueBindable(std::make_unique<PixelConstantBuffer<PixelShaderConstants>>(gfx, cb));
}

void Pyramid::Update(float dt) noexcept
{
    roll += droll * dt;
    pitch += dpitch * dt;
    yaw += dyaw * dt;
    theta += dtheta * dt;
    phi += dphi * dt;
    chi += dchi * dt;
}

DirectX::XMMATRIX Pyramid::GetTransformXM() const noexcept
{
    return DirectX::XMMatrixRotationRollPitchYaw(pitch, yaw, roll) *
        DirectX::XMMatrixTranslation(r, 0.0f, 0.0f) *
        DirectX::XMMatrixRotationRollPitchYaw(theta, phi, chi) *
        DirectX::XMMatrixTranslation(0.0f, 0.0f, 20.0f);
}