#include "Renderable/Cube.h"
#include "Bindable/BindableBase.h"
#include "Bindable/BindableCache.h"
#include "Geometry/GeometryFactory.h"
#include "imgui/imgui.h"
#include <memory>

Cube::Cube(
    Graphics& gfx,
    std::mt19937& rng,
    std::uniform_real_distribution<float>& adist,
    std::uniform_real_distribution<float>& ddist,
    std::uniform_real_distribution<float>& odist,
    std::uniform_real_distribution<float>& rdist,
    std::uniform_real_distribution<float>& bdist,
    DirectX::XMFLOAT3 materialColor
) : RenderableTestObject(rng, adist, ddist, odist, rdist)
{
    auto cubeMesh = GeometryFactory::CreateIndependentCube<VertexPositionNormal>();

    // Vertex Shader - shared
    auto vs = AddSharedBindable<VertexShader>(gfx, "box_vs", L"shaders/Output/PhongVS.cso");
    auto pvsbc = vs->GetByteCode();

    // Pixel Shader - shared
    AddSharedBindable<PixelShader>(gfx, "box_ps", L"shaders/Output/PhongPS.cso");

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
    AddSharedBindable<InputLayout>(gfx, "position_normal_layout", layout, pvsbc);

    materialConstantBuffer.color = materialColor;
    AddUniqueBindable(std::make_unique<PixelConstantBuffer<PSMaterialConstantBuffer>>(gfx, materialConstantBuffer, 1u));

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

DirectX::XMMATRIX Cube::GetTransformXM() const noexcept
{
    return DirectX::XMMatrixRotationRollPitchYaw(pitch, yaw, roll) *
        DirectX::XMMatrixTranslation(r, 0.0f, 0.0f) *
        DirectX::XMMatrixRotationRollPitchYaw(theta, phi, chi);
}

bool Cube::SpawnControlWindow(int id, Graphics& gfx) noexcept
{

    bool dirty = false;
    bool open = true;
    if (ImGui::Begin(("Box " + std::to_string(id)).c_str(), &open))
    {
        ImGui::Text("Material Properties");
        const auto materialColor = ImGui::ColorEdit3("Material Color", &materialConstantBuffer.color.x);
        const auto specularIntensity = ImGui::SliderFloat("Specular Intensity", &materialConstantBuffer.specularIntensity, 0.0f, 2.0f, "%.2f", ImGuiSliderFlags_Logarithmic);
        const auto specularPower = ImGui::SliderFloat("Specular Power", &materialConstantBuffer.specularPower, 1.0f, 128.0f, "%.1f", ImGuiSliderFlags_Logarithmic);
        dirty = materialColor || specularIntensity || specularPower;

        ImGui::Text("Position");
        ImGui::SliderFloat("R", &r, 0.0f, 80.0f, "%.1f");
        ImGui::SliderAngle("Theta", &theta, -180.0f, 180.0f);
        ImGui::SliderAngle("Phi", &phi, -180.0f, 180.0f);
        ImGui::Text("Orientation");
        ImGui::SliderAngle("Roll", &roll, -180.0f, 180.0f);
        ImGui::SliderAngle("Pitch", &pitch, -180.0f, 180.0f);
        ImGui::SliderAngle("Yaw", &yaw, -180.0f, 180.0f);
    }

	ImGui::End();

    if (dirty)
    {
        SyncMaterial(gfx);
    }

    return open;
}

void Cube::SyncMaterial(Graphics& gfx) noexcept(!_DEBUG)
{
    // Update the material constant buffer
	auto pixelShaderConstantBuffer = QueryUniqueBindable<PixelConstantBuffer<PSMaterialConstantBuffer>>();

    if (pixelShaderConstantBuffer != nullptr)
    {
        pixelShaderConstantBuffer->Update(gfx, materialConstantBuffer);
    }
}
