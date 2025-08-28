#include "Renderable/Model/Model.h"
#include "Bindable/BindableCommon.h"
#include "Geometry/Vertex.h"
#include <cassert>
#include <imgui/imgui.h>

Node::Node(const std::string& name, std::vector<Mesh*> meshesIn, const DirectX::XMMATRIX& transform)
    : meshes(std::move(meshesIn)), name(name)
{
    DirectX::XMStoreFloat4x4(&localTransform, transform);
}

void Node::Render(Graphics& gfx, DirectX::FXMMATRIX parentTransform) const noexcept
{
    const auto accumulatedTransform = DirectX::XMLoadFloat4x4(&localTransform) * parentTransform;
    for (auto* m : meshes)
    {
        m->Draw(gfx, accumulatedTransform);
    }
    for (const auto& child : children)
    {
        child->Render(gfx, accumulatedTransform);
    }
}

/// <summary>
/// Renders the node hierarchy as an ImGui tree for debugging purposes.
/// </summary>
void Node::RenderTree() const noexcept
{
    if (ImGui::TreeNode(name.c_str()))
    {
        for (const auto& child : children)
        {
            child->RenderTree();
        }
        ImGui::TreePop();
	}
}

void Node::AddChild(std::unique_ptr<Node> child) noexcept
{
    children.push_back(std::move(child));
}

class ModelWindow
{
public:
    void Render(const char* windowName, const Node& root) noexcept
    {
        if (ImGui::Begin("Model"))
        {
            ImGui::Columns(2, nullptr, true);
            root.RenderTree();
            ImGui::NextColumn();
            ImGui::Text("Orientation");
            ImGui::SliderAngle("Roll", &modelPose.roll, -180.0f, 180.0f);
            ImGui::SliderAngle("Pitch", &modelPose.pitch, -180.0f, 180.0f);
            ImGui::SliderAngle("Yaw", &modelPose.yaw, -180.0f, 180.0f);
            ImGui::Text("Position");
            ImGui::SliderFloat("X", &modelPose.x, -20.0f, 20.0f);
            ImGui::SliderFloat("Y", &modelPose.y, -20.0f, 20.0f);
            ImGui::SliderFloat("Z", &modelPose.z, -20.0f, 20.0f);
        }
        ImGui::End();
    }

    DirectX::XMMATRIX GetTransform() const noexcept
    {
        return 
            DirectX::XMMatrixRotationRollPitchYaw(modelPose.pitch, modelPose.yaw, modelPose.roll)
            * DirectX::XMMatrixTranslation(modelPose.x, modelPose.y, modelPose.z);
    }
private:
    struct
    {
        float roll = 0.0f;
        float pitch = 0.0f;
        float yaw = 0.0f;
        float x = 0.0f;
        float y = 0.0f;
        float z = 0.0f;
    } modelPose;
};

Model::Model(Graphics& gfx, const std::string& filePath) : pWindow(std::make_unique<ModelWindow>())
{
    Assimp::Importer importer;
    const aiScene* scene = importer.ReadFile(
        filePath.c_str(),
        aiProcess_Triangulate |
        aiProcess_JoinIdenticalVertices
    );

    for (unsigned int i = 0; i < scene->mNumMeshes; ++i)
    {
        meshes.push_back(BuildMesh(gfx, *scene->mMeshes[i]));
    }
    root = BuildNode(*scene->mRootNode);
}

Model::~Model() noexcept {};

void Model::Render(Graphics& gfx) const noexcept
{
    root->Render(gfx, pWindow->GetTransform());
}

void Model::ShowModelControlWindow(const char* windowName) noexcept
{
    pWindow->Render(windowName, *root);
}

std::unique_ptr<Mesh> Model::BuildMesh(Graphics& gfx, const aiMesh& mesh)
{
    using namespace DirectX;

    D3::VertexLayout layout;
    layout.Append(D3::VertexLayout::Position3D)
          .Append(D3::VertexLayout::Normal);
    D3::VertexBuffer vbuf(std::move(layout));
    for (unsigned int i = 0; i < mesh.mNumVertices; ++i)
    {
        vbuf.EmplaceBack(
            *reinterpret_cast<const XMFLOAT3*>(&mesh.mVertices[i]),
            *reinterpret_cast<const XMFLOAT3*>(&mesh.mNormals[i])
        );
    }

    std::vector<unsigned short> indices;
    indices.reserve(mesh.mNumFaces * 3);
    for (unsigned int i = 0; i < mesh.mNumFaces; ++i)
    {
        const auto& f = mesh.mFaces[i];
        assert(f.mNumIndices == 3);
        indices.push_back(f.mIndices[0]);
        indices.push_back(f.mIndices[1]);
        indices.push_back(f.mIndices[2]);
    }

    std::vector<std::unique_ptr<Bindable>> binds;

    // VB/IB
    binds.push_back(std::make_unique<VertexBuffer>(gfx, vbuf));
    binds.push_back(std::make_unique<IndexBuffer>(gfx, indices));

    // Shaders
    auto pvs = std::make_unique<VertexShader>(gfx, L"shaders/Output/PhongVS.cso");
    auto vsbc = pvs->GetByteCode();
    binds.push_back(std::move(pvs));
    binds.push_back(std::make_unique<PixelShader>(gfx, L"shaders/Output/PhongPS.cso"));

    // Input layout from dynamic layout
    binds.push_back(std::make_unique<InputLayout>(gfx, vbuf.GetLayout().GetD3DLayout(), vsbc));

    // Primitive topology
    binds.push_back(std::make_unique<Topology>(gfx, D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST));

    struct PSMaterial
    {
        DirectX::XMFLOAT3 color = { 0.6f,0.6f,0.8f };
        float specularIntensity = 0.6f;
        float specularPower = 30.0f;
        float padding[3] = {};
    } pm;
    binds.push_back(std::make_unique<PixelConstantBuffer<PSMaterial>>(gfx, pm, 1u));

    return std::make_unique<Mesh>(gfx, std::move(binds));
}

std::unique_ptr<Node> Model::BuildNode(const aiNode& node) noexcept
{
    using namespace DirectX;
    const auto transform = XMMatrixTranspose(
        XMLoadFloat4x4(reinterpret_cast<const XMFLOAT4X4*>(&node.mTransformation))
    );

    std::vector<Mesh*> collect;
    collect.reserve(node.mNumMeshes);
    for (unsigned int i = 0; i < node.mNumMeshes; ++i)
    {
        collect.push_back(meshes.at(node.mMeshes[i]).get());
    }

    auto created = std::make_unique<Node>(node.mName.C_Str(), std::move(collect), transform);
    for (unsigned int i = 0; i < node.mNumChildren; ++i)
    {
        created->AddChild(BuildNode(*node.mChildren[i]));
    }
    return created;
}


