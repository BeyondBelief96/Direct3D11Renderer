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
) : RenderableTestObject(rng, adist, ddist, odist, rdist)
{
    // Create a custom pyramid mesh with proper normals
    std::vector<VertexPositionNormal> vertices;
    std::vector<unsigned short> indices;

    const float halfHeight = height / 2.0f;

    // Create the top vertex (apex of pyramid)
    VertexPositionNormal apexVertex;
    apexVertex.position = { 0.0f, 0.0f, halfHeight };

    // Create base vertices
    std::vector<VertexPositionNormal> baseVertices;
    for (int i = 0; i < sides; i++)
    {
        const float angle = (2.0f * PI * i) / sides;
        VertexPositionNormal vertex;
        vertex.position = {
            radius * std::cos(angle),
            radius * std::sin(angle),
            -halfHeight
        };
        baseVertices.push_back(vertex);
    }

    // Create triangular faces (sides of the pyramid)
    for (int i = 0; i < sides; i++)
    {
        // Calculate face normal using cross product
        DirectX::XMVECTOR v0 = DirectX::XMLoadFloat3(&baseVertices[i].position);
        DirectX::XMVECTOR v1 = DirectX::XMLoadFloat3(&baseVertices[(i + 1) % sides].position);
        DirectX::XMVECTOR v2 = DirectX::XMLoadFloat3(&apexVertex.position);

        DirectX::XMVECTOR edge1 = DirectX::XMVectorSubtract(v1, v0);
        DirectX::XMVECTOR edge2 = DirectX::XMVectorSubtract(v2, v0);
        DirectX::XMVECTOR normal = DirectX::XMVector3Normalize(
            DirectX::XMVector3Cross(edge1, edge2)
        );

        DirectX::XMFLOAT3 faceNormal;
        DirectX::XMStoreFloat3(&faceNormal, normal);

        // Add face vertices with correct normals
        // Base vertices for this face
        VertexPositionNormal baseVertex1 = baseVertices[i];
        baseVertex1.normal = faceNormal;

        VertexPositionNormal baseVertex2 = baseVertices[(i + 1) % sides];
        baseVertex2.normal = faceNormal;

        // Apex vertex for this face
        VertexPositionNormal faceApex = apexVertex;
        faceApex.normal = faceNormal;

        // Add vertices
        unsigned short baseIndex1 = static_cast<unsigned short>(vertices.size());
        vertices.push_back(baseVertex1);

        unsigned short baseIndex2 = static_cast<unsigned short>(vertices.size());
        vertices.push_back(baseVertex2);

        unsigned short apexIndex = static_cast<unsigned short>(vertices.size());
        vertices.push_back(faceApex);

        // Add indices for this face
        indices.push_back(baseIndex1);
        indices.push_back(baseIndex2);
        indices.push_back(apexIndex);
    }

    // Create base of pyramid (assuming it's a flat base)
    // Calculate base center
    VertexPositionNormal baseCenter;
    baseCenter.position = { 0.0f, 0.0f, -halfHeight };
    baseCenter.normal = { 0.0f, 0.0f, -1.0f }; // Base normal points down

    unsigned short baseCenterIndex = static_cast<unsigned short>(vertices.size());
    vertices.push_back(baseCenter);

    // Add base vertices with correct normals
    for (int i = 0; i < sides; i++)
    {
        VertexPositionNormal baseVertex = baseVertices[i];
        baseVertex.normal = { 0.0f, 0.0f, -1.0f }; // Base normal points down

        unsigned short baseIndex = static_cast<unsigned short>(vertices.size());
        vertices.push_back(baseVertex);

        // Add triangle for this section of the base
        indices.push_back(baseCenterIndex);
        indices.push_back(baseIndex);
        indices.push_back(static_cast<unsigned short>(vertices.size()) % (sides + 1) + baseCenterIndex);
    }

    // Fix the last base triangle
    indices[indices.size() - 1] = baseCenterIndex + 1;

    // Create a mesh from these custom vertices and indices
    GeometryMesh<VertexPositionNormal> pyramidMesh(std::move(vertices), std::move(indices));

    // Bind vertex shader
    auto vs = AddSharedBindable<VertexShader>(gfx, "vs_pyramid_phong", L"PhongVS.cso");
    auto pvs = vs->GetByteCode();

    // Bind Pixel Shader
    AddSharedBindable<PixelShader>(gfx, "ps_pyramid_phong", L"PhongPS.cso");

    // Bind Vertex Buffer
    std::string vbKey = "pyramid_vertices_phong_" + std::to_string(sides) + "_" + std::to_string(radius);
    AddSharedBindable<VertexBuffer>(gfx, vbKey, pyramidMesh.vertices);

    // Bind Index Buffer
    std::string ibKey = "pyramid_indices_phong_" + std::to_string(sides);
    AddSharedBindable<IndexBuffer>(gfx, ibKey, pyramidMesh.indices);

    // Create input layout
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

    // Generate a solid color for the pyramid
    std::uniform_real_distribution<float> cdist(0.0f, 1.0f);
    objectConstantBuffer.materialColor = { cdist(rng), cdist(rng), cdist(rng) };

    // Add the pixel shader constant buffer
    AddUniqueBindable(std::make_unique<PixelConstantBuffer<PSObjectConstantBuffer>>(gfx, objectConstantBuffer, 1u));

    // Create the transform constant buffer
    AddUniqueBindable(std::make_unique<TransformConstantBuffer>(gfx, *this));
}

DirectX::XMMATRIX Pyramid::GetTransformXM() const noexcept
{
    return DirectX::XMMatrixRotationRollPitchYaw(pitch, yaw, roll) *
        DirectX::XMMatrixTranslation(r, 0.0f, 0.0f) *
        DirectX::XMMatrixRotationRollPitchYaw(theta, phi, chi);
}