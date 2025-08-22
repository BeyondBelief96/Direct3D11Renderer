#include "Renderable/Sphere.h"

Sphere::Sphere(
    Graphics& gfx,
    std::mt19937& rng,
    std::uniform_real_distribution<float>& adist,
    std::uniform_real_distribution<float>& ddist,
    std::uniform_real_distribution<float>& odist,
    std::uniform_real_distribution<float>& rdist,
    float radius, int tessellation)
    : RenderableTestObject(rng, adist, ddist, odist, rdist)
{
    // Create a sphere mesh with normals
    auto sphereMesh = GeometryFactory::CreateSphere<VertexPositionNormal>(radius, tessellation);

    // Bind vertex shader
    auto vs = AddSharedBindable<VertexShader>(gfx, "vs_sphere_phong", L"shaders/Output/PhongVS.cso");
    auto pvs = vs->GetByteCode();

    // Bind Pixel Shader
    AddSharedBindable<PixelShader>(gfx, "ps_sphere_phong", L"shaders/Output/PhongPS.cso");

    // Bind Vertex Buffer
    std::string vbKey = "sphere_vertices_phong_" + std::to_string(tessellation);
    AddSharedBindable<VertexBuffer>(gfx, vbKey, sphereMesh.vertices);

    // Bind Index Buffer
    std::string ibKey = "sphere_indices_phong_" + std::to_string(tessellation);
    AddSharedBindable<IndexBuffer>(gfx, ibKey, sphereMesh.indices);

    // Create input layout for position and normal
    const std::vector<D3D11_INPUT_ELEMENT_DESC> layout =
    {
        { "Position", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
        { "Normal", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 },
    };
    AddSharedBindable<InputLayout>(gfx, "position_normal_layout", layout, pvs);

    // Create topology
    AddSharedBindable<Topology>(gfx, "triangle_list", D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

    // Create the material constant buffer
    struct PSObjectConstantBuffer
    {
        alignas(16) DirectX::XMFLOAT3 materialColor;
        float specularIntensity = 0.6f;
        float specularPower = 30.0f;
        float padding[2] = {};
    } objectConstantBuffer;

    // Generate random color for the sphere
    std::uniform_real_distribution<float> cdist(0.0f, 1.0f);
    objectConstantBuffer.materialColor = { cdist(rng), cdist(rng), cdist(rng) };

    // Add the pixel shader constant buffer
    AddUniqueBindable(std::make_unique<PixelConstantBuffer<PSObjectConstantBuffer>>(gfx, objectConstantBuffer, 1u));

    // Create the transform constant buffer
    AddUniqueBindable(std::make_unique<TransformConstantBuffer>(gfx, *this));
}

DirectX::XMMATRIX Sphere::GetTransformXM() const noexcept
{
    return DirectX::XMMatrixRotationRollPitchYaw(pitch, yaw, roll) *
        DirectX::XMMatrixTranslation(r, 0.0f, 0.0f) *
        DirectX::XMMatrixRotationRollPitchYaw(theta, phi, chi);
}