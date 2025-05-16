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
) :RenderableTestObject(rng, adist, ddist, odist, rdist)
{
    // Create a plane with texture coordinates
    auto planeMesh = GeometryFactory::CreatePlane<VertexPositionTexture>(width, height, divisionsX, divisionsY);
    planeMesh.SetFlatNormals();
    // Add Texture
    AddSharedBindable<Texture>(gfx, "kappa_texture", L"assets/kappa50.png");

	// Add Sampler
    AddSharedBindable<Sampler>(gfx, "plane_sampler");

    // Bind vertex shader
    auto vs = AddSharedBindable<VertexShader>(gfx, "vs_plane", L"TextureVS.cso");
    auto pvs = vs->GetByteCode();

    // Bind Pixel Shader
    auto ps = AddSharedBindable<PixelShader>(gfx, "ps_plane", L"TexturePS.cso");

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
		{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 },
    };
    AddSharedBindable<InputLayout>(gfx, "plane_input_layout", layout, pvs);

    // Create topology
    AddSharedBindable<Topology>(gfx, "plane_topology", D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

    // Create the transform constant buffer
    AddUniqueBindable(std::make_unique<TransformConstantBuffer>(gfx, *this));
}

DirectX::XMMATRIX Plane::GetTransformXM() const noexcept
{
    return DirectX::XMMatrixRotationRollPitchYaw(pitch, yaw, roll) *
        DirectX::XMMatrixTranslation(r, 0.0f, 0.0f) *
        DirectX::XMMatrixRotationRollPitchYaw(theta, phi, chi);
}