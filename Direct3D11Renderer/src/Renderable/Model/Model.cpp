#include "Renderable/Model/Model.h"
#include "Bindable/BindableCommon.h"
#include "Exceptions/ModelException.h"
#include "Geometry/Vertex.h"
#include <cassert>
#include <imgui/imgui.h>
#include <unordered_map>

// -----------------------------------------------------------------------------
// Node Class - Represents a node in the model's scene graph hierarchy.
// -----------------------------------------------------------------------------

Node::Node(int id, const std::string& name, std::vector<Mesh*> meshesIn, const DirectX::XMMATRIX& transformIn)
    : meshes(std::move(meshesIn)), name(name), id(id)
{
    DirectX::XMStoreFloat4x4(&transform, transformIn);
    DirectX::XMStoreFloat4x4(&appliedTransform, DirectX::XMMatrixIdentity());
}

void Node::Render(Graphics& gfx, DirectX::FXMMATRIX parentTransform) const noexcept
{
    const auto built = 
        DirectX::XMLoadFloat4x4(&appliedTransform) * 
        DirectX::XMLoadFloat4x4(&transform) * 
        parentTransform;
        
    for (auto* m : meshes)
    {
        m->Draw(gfx, built);
    }
    for (const auto& child : children)
    {
        child->Render(gfx, built);
    }
}

/// <summary>
/// Renders the node hierarchy as an ImGui tree for debugging purposes.
/// </summary>
void Node::RenderTree(std::optional<int>& selectedIndex, Node*& pSelectedNode) const noexcept
{
    const auto nodeFlags = ImGuiTreeNodeFlags_OpenOnArrow
        | ((GetId() == selectedIndex.value_or(-1)) ? ImGuiTreeNodeFlags_Selected : 0)
        | ((children.size() == 0) ? ImGuiTreeNodeFlags_Leaf : 0);

    const auto expanded = ImGui::TreeNodeEx((void*)(intptr_t)GetId(), nodeFlags, name.c_str());

    if (ImGui::IsItemClicked())
    {
        selectedIndex = GetId();
        pSelectedNode = const_cast<Node*>(this);
    }
    if (expanded)
    {
        for (const auto& child : children)
        {
            child->RenderTree(selectedIndex, pSelectedNode);
        }
        ImGui::TreePop();
	}
}

void Node::AddChild(std::unique_ptr<Node> child) noexcept
{
    children.push_back(std::move(child));
}

void Node::SetAppliedTransform(DirectX::FXMMATRIX transform) noexcept
{
    DirectX::XMStoreFloat4x4(&appliedTransform, transform);
}

int Node::GetId() const noexcept
{
    return id;
}

// -----------------------------------------------------------------------------
// Model Class - Represents a 3D model composed of meshes and a node hierarchy.
// -----------------------------------------------------------------------------

/// <summary>
/// Represents a window for controlling the model's pose (position and orientation) and displaying its node hierarchy.
/// This class uses ImGui for rendering the UI elements.
/// This class is private to the Model class and is not exposed externally.
/// Used for debugging and visualization purposes.
/// </summary>
class ModelWindow
{
public:
    void Render(const char* windowName, const Node& root) noexcept
    {
        // Default window name to "Model" if none provided
        windowName = windowName ? windowName : "Model";
        
        if (ImGui::Begin(windowName))
        {
            ImGui::Columns(2, nullptr, true);
            root.RenderTree(selectedIndex, pSelectedNode);
            
            ImGui::NextColumn();
            ImGui::Text("Orientation");
            
            if (pSelectedNode != nullptr)
            {
                // Get or create transform parameters for the selected node
                auto& transform = transforms[*selectedIndex];
                
                ImGui::SliderAngle("Roll", &transform.roll, -180.0f, 180.0f);
                ImGui::SliderAngle("Pitch", &transform.pitch, -180.0f, 180.0f);
                ImGui::SliderAngle("Yaw", &transform.yaw, -180.0f, 180.0f);
                ImGui::Text("Position");
                ImGui::SliderFloat("X", &transform.x, -20.0f, 20.0f);
                ImGui::SliderFloat("Y", &transform.y, -20.0f, 20.0f);
                ImGui::SliderFloat("Z", &transform.z, -20.0f, 20.0f);
            }
            else
            {
                // Use model-wide pose when no node is selected
                ImGui::SliderAngle("Roll", &modelPose.roll, -180.0f, 180.0f);
                ImGui::SliderAngle("Pitch", &modelPose.pitch, -180.0f, 180.0f);
                ImGui::SliderAngle("Yaw", &modelPose.yaw, -180.0f, 180.0f);
                ImGui::Text("Position");
                ImGui::SliderFloat("X", &modelPose.x, -20.0f, 20.0f);
                ImGui::SliderFloat("Y", &modelPose.y, -20.0f, 20.0f);
                ImGui::SliderFloat("Z", &modelPose.z, -20.0f, 20.0f);
            }
        }
        ImGui::End();
    }

    DirectX::XMMATRIX GetTransform() const noexcept
    {
        if (selectedIndex.has_value() && pSelectedNode != nullptr)
        {
            const auto& transform = transforms.at(*selectedIndex);
            return 
                DirectX::XMMatrixRotationRollPitchYaw(transform.pitch, transform.yaw, transform.roll)
                * DirectX::XMMatrixTranslation(transform.x, transform.y, transform.z);
        }
        
        return 
            DirectX::XMMatrixRotationRollPitchYaw(modelPose.pitch, modelPose.yaw, modelPose.roll)
            * DirectX::XMMatrixTranslation(modelPose.x, modelPose.y, modelPose.z);
    }
    
    Node* GetSelectedNode() const noexcept
    {
        return pSelectedNode;
    }
    
private:
    std::optional<int> selectedIndex;
    Node* pSelectedNode = nullptr;
    
    struct TransformParameters
    {
        float roll = 0.0f;
        float pitch = 0.0f;
        float yaw = 0.0f;
        float x = 0.0f;
        float y = 0.0f;
        float z = 0.0f;
    };
    
    std::unordered_map<int, TransformParameters> transforms;
    
    // Default model-wide transformation
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
        aiProcess_ConvertToLeftHanded |
        aiProcess_GenNormals |
        aiProcess_JoinIdenticalVertices
    );

    if (scene == nullptr)
    {
        throw ModelException(__LINE__, __FILE__, importer.GetErrorString());
    }

    for (unsigned int i = 0; i < scene->mNumMeshes; ++i)
    {
        meshes.push_back(BuildMesh(gfx, *scene->mMeshes[i]));
    }
    int nextId = 0;
    root = BuildNode(nextId, *scene->mRootNode);
}

Model::~Model() noexcept {};

void Model::Render(Graphics& gfx) const noexcept
{
    // Apply transform to selected node if any
    if (auto node = pWindow->GetSelectedNode())
    {
        node->SetAppliedTransform(pWindow->GetTransform());
    }
    
    // Render the model with identity as initial transform
    root->Render(gfx, DirectX::XMMatrixIdentity());
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

std::unique_ptr<Node> Model::BuildNode(int& nextId, const aiNode& node) noexcept
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

    auto created = std::make_unique<Node>(nextId++, node.mName.C_Str(), std::move(collect), transform);
    for (unsigned int i = 0; i < node.mNumChildren; ++i)
    {
        created->AddChild(BuildNode(nextId, *node.mChildren[i]));
    }
    return created;
}


