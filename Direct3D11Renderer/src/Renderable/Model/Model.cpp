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
void Node::RenderTree(Node*& pSelectedNode) const noexcept
{
    const auto selectedId = (pSelectedNode == nullptr) ? -1 : pSelectedNode->GetId();
    const auto nodeFlags = ImGuiTreeNodeFlags_OpenOnArrow
        | ((GetId() == selectedId) ? ImGuiTreeNodeFlags_Selected : 0)
        | ((children.size() == 0) ? ImGuiTreeNodeFlags_Leaf : 0);

    const auto expanded = ImGui::TreeNodeEx((void*)(intptr_t)GetId(), nodeFlags, name.c_str());

    if (ImGui::IsItemClicked())
    {
        pSelectedNode = const_cast<Node*>(this);
    }
    if (expanded)
    {
        for (const auto& child : children)
        {
            child->RenderTree(pSelectedNode);
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
            root.RenderTree(pSelectedNode);
            
            ImGui::NextColumn();
            ImGui::Text("Orientation");
            
            if (pSelectedNode != nullptr)
            {
                // Get or create transform parameters for the selected node
                auto& transform = transforms[pSelectedNode->GetId()];
                
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
        if (pSelectedNode != nullptr)
        {
            const auto& transform = transforms.at(pSelectedNode->GetId());
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
        meshes.push_back(BuildMesh(gfx, *scene->mMeshes[i], scene->mMaterials));
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

std::unique_ptr<Mesh> Model::BuildMesh(Graphics& gfx, const aiMesh& mesh, const aiMaterial* const* pMaterials)
{
    using namespace DirectX;

	// Define the vertex layout
    D3::VertexLayout layout;
    layout.Append(D3::VertexLayout::Position3D)
          .Append(D3::VertexLayout::Normal)
          .Append(D3::VertexLayout::Texture2D);

	// Create vertex buffer and populate it
    D3::VertexBuffer vbuf(std::move(layout));
    for (unsigned int i = 0; i < mesh.mNumVertices; ++i)
    {
        vbuf.EmplaceBack(
            *reinterpret_cast<const XMFLOAT3*>(&mesh.mVertices[i]),
            *reinterpret_cast<const XMFLOAT3*>(&mesh.mNormals[i]),
			*reinterpret_cast<const XMFLOAT2*>(&mesh.mTextureCoords[0][i])
        );
    }

	// Create index buffer and populate it
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

    std::vector<std::shared_ptr<Bindable>> bindables;
    bool hasSpecularMap = false;
    float shininess = 35.0f;
    const auto basePath = "assets/models/nano_textured/";
    if (mesh.mMaterialIndex >= 0)
    {
		auto& material = *pMaterials[mesh.mMaterialIndex];

        aiString textureFileName;
		// Diffuse texture
        material.GetTexture(aiTextureType_DIFFUSE, 0, &textureFileName);
        bindables.push_back(Texture::Resolve(gfx, basePath + std::string(textureFileName.C_Str()), 0));

		// Specular texture
        if (material.GetTexture(aiTextureType_SPECULAR, 0, &textureFileName) == aiReturn_SUCCESS)
        {

            bindables.push_back(Texture::Resolve(gfx, basePath + std::string(textureFileName.C_Str()), 1));
            hasSpecularMap = true;
        }
        else
        {
            material.Get(AI_MATKEY_SHININESS, shininess);
        }
        
		// Sampler
        bindables.push_back(Sampler::Resolve(gfx));
    }

	const auto meshTag = basePath + std::string("%") + mesh.mName.C_Str();

    // VB/IB
    bindables.push_back(VertexBuffer::Resolve(gfx, meshTag, vbuf));
    bindables.push_back(IndexBuffer::Resolve(gfx, meshTag, indices));

    // Shaders
    auto pvs = VertexShader::Resolve(gfx, "shaders/Output/PhongVS.cso");
    auto vsbc = static_cast<VertexShader&>(*pvs).GetByteCode();
    bindables.push_back(std::move(pvs));

    if (hasSpecularMap)
    {
        bindables.push_back(PixelShader::Resolve(gfx, "shaders/Output/PhongPSSpecularMap.cso"));
    }
    else
    {
        bindables.push_back(PixelShader::Resolve(gfx, "shaders/Output/PhongPS.cso"));
        struct PSMaterial
        {
            float specularIntensity = 1.6f;
            float specularPower = 0.0f;
            float padding[2] = {};
        } pm;

        pm.specularPower = shininess;
        bindables.push_back(PixelConstantBuffer<PSMaterial>::Resolve(gfx, pm, 1u));
	}

    // Input layout from dynamic layout
    bindables.push_back(InputLayout::Resolve(gfx, vbuf.GetLayout(), vsbc));

    // Primitive topology
    bindables.push_back(Topology::Resolve(gfx, D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST));

    return std::make_unique<Mesh>(gfx, std::move(bindables));
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


