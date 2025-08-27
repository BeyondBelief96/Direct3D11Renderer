#include "Renderable/TexturedCube.h"
#include "Bindable/BindableCommon.h"

TexturedCube::TexturedCube(
    Graphics& gfx,
    std::mt19937& rng,
    std::uniform_real_distribution<float>& adist,
    std::uniform_real_distribution<float>& ddist,
    std::uniform_real_distribution<float>& odist,
    std::uniform_real_distribution<float>& rdist,
    float size, const std::wstring& textureFilename)
	: RenderableTestObject(rng, adist, ddist, odist, rdist)
{
    // Create a cube mesh with texture coordinates
    auto cubeMesh = GeometryFactory::CreateIndependentTexturedCube<VertexPositionNormalTexture>(size);
    // Vertex Shader
    auto vertexShader = AddSharedBindable<VertexShader>(gfx, "textured_vs", L"shaders/Output/TexturedPhongVS.cso");
    auto vertexShaderByteCode = vertexShader->GetByteCode();

    // Pixel Shader
    AddSharedBindable<PixelShader>(gfx, "textured_ps", L"shaders/Output/TexturedPhongPS.cso");

    // Vertex Buffer
    AddSharedBindable<VertexBuffer>(gfx, "textured_cube_vb" + std::to_string(size), cubeMesh.vertices);

    // Index Buffer
    AddSharedBindable<IndexBuffer>(gfx, "textured_cube_ib" + std::to_string(size), cubeMesh.indices);

    // Input Layout
    const std::vector<D3D11_INPUT_ELEMENT_DESC> inputLayout =
    {
        {"Position", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{"Normal", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 },
        {"TexCoord", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 24, D3D11_INPUT_PER_VERTEX_DATA, 0}
    };

	AddSharedBindable<InputLayout>(gfx, "position_normal_texture_layout" + std::to_string(size), inputLayout, vertexShaderByteCode);

    // Add Texture and Sampler
	AddSharedBindable<Texture>(gfx, "box_texture" , textureFilename);
	AddSharedBindable<Sampler>(gfx, "box_sampler");

    // Topology
	AddSharedBindable<Topology>(gfx, "triangle_list", D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

    struct PSMaterialConstant
    {
        float specularIntensity = 0.6f;
        float specularPower = 30.0f;
        float padding[2];
    } colorConst;

	// Binding the material constant buffer
	AddUniqueBindable(std::make_unique<PixelConstantBuffer<PSMaterialConstant>>(gfx, colorConst, 1));

    // Transform Constant Buffer
	AddUniqueBindable(std::make_unique<TransformConstantBuffer>(gfx, *this));
}

DirectX::XMMATRIX TexturedCube::GetTransformXM() const noexcept
{
    return DirectX::XMMatrixRotationRollPitchYaw(pitch, yaw, roll) *
        DirectX::XMMatrixTranslation(r, 0.0f, 0.0f) *
        DirectX::XMMatrixRotationRollPitchYaw(theta, phi, chi);
}
