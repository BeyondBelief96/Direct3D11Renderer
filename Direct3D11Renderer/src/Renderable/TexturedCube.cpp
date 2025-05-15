#include "Renderable/TexturedCube.h"
#include "Bindable/BindableBase.h"

TexturedCube::TexturedCube(
    Graphics& gfx,
    std::mt19937& rng,
    std::uniform_real_distribution<float>& adist,
    std::uniform_real_distribution<float>& ddist,
    std::uniform_real_distribution<float>& odist,
    std::uniform_real_distribution<float>& rdist,
    float size, const std::wstring& textureFilename)
    : r(rdist(rng)),
    roll(ddist(rng)),
    pitch(ddist(rng)),
    yaw(ddist(rng)),
    theta(odist(rng)),
    phi(odist(rng)),
    chi(odist(rng)),
    droll(adist(rng)),
    dpitch(adist(rng)),
    dyaw(adist(rng)),
    dtheta(adist(rng)),
    dphi(adist(rng)),
    dchi(adist(rng))
{
    // Create a cube mesh with texture coordinates
    auto cubeMesh = GeometryFactory::CreateTexturedCube<VertexPositionTexture>(size);

    // Vertex Shader
    auto vertexShader = AddSharedBindable<VertexShader>(gfx, "textured_vs", L"TextureVS.cso");
    auto vertexShaderByteCode = vertexShader->GetByteCode();

    // Pixel Shader
    AddSharedBindable<PixelShader>(gfx, "textured_ps", L"TexturePS.cso");

    // Vertex Buffer
    AddSharedBindable<VertexBuffer>(gfx, "textured_cube_vb" + std::to_string(size), cubeMesh.vertices);

    // Index Buffer
    AddSharedBindable<IndexBuffer>(gfx, "textured_cube_ib" + std::to_string(size), cubeMesh.indices);

    // Input Layout
    const std::vector<D3D11_INPUT_ELEMENT_DESC> inputLayout =
    {
        {"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
        { "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0}
    };

	AddSharedBindable<InputLayout>(gfx, "textured_cube_input_layout" + std::to_string(size), inputLayout, vertexShaderByteCode);

    // Add Texture and Sampler
	AddSharedBindable<Texture>(gfx, "box_texture" , textureFilename);
	AddSharedBindable<Sampler>(gfx, "box_sampler");

    // Topology
	AddSharedBindable<Topology>(gfx, "triangle_list", D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

    // Transform Constant Buffer
	AddUniqueBindable(std::make_unique<TransformConstantBuffer>(gfx, *this));
}

void TexturedCube::Update(float dt) noexcept
{
    roll += droll * dt;
    pitch += dpitch * dt;
    yaw += dyaw * dt;
    theta += dtheta * dt;
    phi += dphi * dt;
    chi += dchi * dt;
}

DirectX::XMMATRIX TexturedCube::GetTransformXM() const noexcept
{
    return DirectX::XMMatrixRotationRollPitchYaw(pitch, yaw, roll) *
        DirectX::XMMatrixTranslation(r, 0.0f, 0.0f) *
        DirectX::XMMatrixRotationRollPitchYaw(theta, phi, chi);
}
