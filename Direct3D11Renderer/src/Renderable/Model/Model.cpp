#include "Renderable/Model/Model.h"
#include "Bindable/BindableCommon.h"
#include "Exceptions/ModelException.h"
#include "Geometry/Vertex.h"
#include <cassert>
#include <imgui/imgui.h>
#include <unordered_map>
#include <filesystem>

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

Model::Model(Graphics& gfx, const std::string& modelPath) : pWindow(std::make_unique<ModelWindow>())
{
    Assimp::Importer importer;
    const aiScene* scene = importer.ReadFile(
        modelPath.c_str(),
        aiProcess_Triangulate |
        aiProcess_ConvertToLeftHanded |
        aiProcess_GenNormals |
        aiProcess_JoinIdenticalVertices |
        aiProcess_CalcTangentSpace
    );

    if (scene == nullptr)
    {
        throw ModelException(__LINE__, __FILE__, importer.GetErrorString());
    }

    for (unsigned int i = 0; i < scene->mNumMeshes; ++i)
    {
        meshes.push_back(BuildMesh(gfx, *scene->mMeshes[i], scene->mMaterials, modelPath));
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

std::unique_ptr<Mesh> Model::BuildMesh(
    Graphics& gfx,
    const aiMesh& mesh, 
    const aiMaterial* const* pMaterials,
    const std::filesystem::path& modelPath)
{
    using namespace DirectX;

    std::vector<std::shared_ptr<Bindable>> bindablePtrs;

    const auto rootPath = modelPath.parent_path().string() + "\\";

    bool hasSpecularMap = false;
    bool hasNormalMap = false;
    bool hasDiffuseMap = false;
	bool hasAlphaChannel = false;
    float shininess = 35.0f;
    XMFLOAT4 meshSpecularColor = { 0.18f, 0.18f, 0.18f, 1.0f };
	XMFLOAT4 meshDiffuseColor = { 0.45f, 0.45f, 0.45f, 1.0f };

    const std::string shaderBasePath = "shaders/Output/";

    if (mesh.mMaterialIndex >= 0)
    {
        auto& material = *pMaterials[mesh.mMaterialIndex];
        aiString textureFileName;

        if(material.GetTexture(aiTextureType_DIFFUSE, 0, &textureFileName) == aiReturn_SUCCESS)
        {
            bindablePtrs.push_back(Texture::Resolve(gfx, rootPath + textureFileName.C_Str(), 0u));
			hasDiffuseMap = true;
        }
        else
        {
			material.Get(AI_MATKEY_COLOR_DIFFUSE, reinterpret_cast<aiColor3D&>(meshDiffuseColor));
        }

        if (material.GetTexture(aiTextureType_SPECULAR, 0, &textureFileName) == aiReturn_SUCCESS)
        {
			auto specularTexture = Texture::Resolve(gfx, rootPath + textureFileName.C_Str(), 1u);
            hasAlphaChannel = specularTexture->AlphaChannelLoaded();
            bindablePtrs.push_back(std::move(specularTexture));
            hasSpecularMap = true;
        }
        else
        {
			material.Get(AI_MATKEY_COLOR_SPECULAR, reinterpret_cast<aiColor3D&>(meshSpecularColor));
        }
        if (!hasAlphaChannel)
        {
			material.Get(AI_MATKEY_SHININESS, hasAlphaChannel);
        }

        if(material.GetTexture(aiTextureType_NORMALS, 0, &textureFileName) == aiReturn_SUCCESS)
        {
			auto normalMap = Texture::Resolve(gfx, rootPath + textureFileName.C_Str(), 2u);
			hasAlphaChannel = normalMap->AlphaChannelLoaded();
            bindablePtrs.push_back(std::move(normalMap));
            hasNormalMap = true;
		}

        if(hasDiffuseMap || hasSpecularMap || hasNormalMap)
        {
            bindablePtrs.push_back(Sampler::Resolve(gfx));
		}

        const std::string meshTag = modelPath.string() + "%" + mesh.mName.C_Str();
        const float scale = 6.0f;

        if (hasDiffuseMap && hasNormalMap && hasSpecularMap)
        {
            D3::VertexLayout layout;
            layout.Append(D3::VertexLayout::Position3D)
                .Append(D3::VertexLayout::Normal)
                .Append(D3::VertexLayout::Tangent)
                .Append(D3::VertexLayout::Bitangent)
                .Append(D3::VertexLayout::Texture2D);
            D3::VertexBuffer vertexBuffer(layout);

            for (unsigned int i = 0; i < mesh.mNumVertices; i++)
            {
                vertexBuffer.EmplaceBack(
                    XMFLOAT3(mesh.mVertices[i].x * scale, mesh.mVertices[i].y * scale, mesh.mVertices[i].z * scale),
                    *reinterpret_cast<XMFLOAT3*>(&mesh.mNormals[i]),
                    *reinterpret_cast<XMFLOAT3*>(&mesh.mTangents[i]),
                    *reinterpret_cast<XMFLOAT3*>(&mesh.mBitangents[i]),
                    *reinterpret_cast<XMFLOAT2*>(&mesh.mTextureCoords[0][i])
                );
            }

            std::vector<unsigned short> indices;
            indices.reserve(mesh.mNumFaces * 3);
            for(unsigned int i = 0; i < mesh.mNumFaces; i++)
            {
                const auto& face = mesh.mFaces[i];
                assert(face.mNumIndices == 3);
                indices.push_back(static_cast<unsigned short>(face.mIndices[0]));
                indices.push_back(static_cast<unsigned short>(face.mIndices[1]));
                indices.push_back(static_cast<unsigned short>(face.mIndices[2]));
			}

            bindablePtrs.push_back(VertexBuffer::Resolve(gfx, meshTag, vertexBuffer));
			bindablePtrs.push_back(IndexBuffer::Resolve(gfx, meshTag, indices));

			auto vertexShader = VertexShader::Resolve(gfx, shaderBasePath + "BlinnPhong_NormalMapped_VS.cso");
            auto vertexShaderByteCode = vertexShader->GetByteCode();
            bindablePtrs.push_back(std::move(vertexShader));
			bindablePtrs.push_back(PixelShader::Resolve(gfx, shaderBasePath + "BlinnPhong_SpecularNormalMapped_PS.cso"));
			bindablePtrs.push_back(InputLayout::Resolve(gfx, vertexBuffer.GetLayout(), vertexShaderByteCode));

            struct PixelShaderMaterialConstantBuffer
            {
                BOOL hasAlphaChannel = FALSE;
				BOOL specularMapEnabled = TRUE;
                float specularPowerConstant = 3.1f;
				float padding[1] = { 0.0f };
            } pmc;

            pmc.specularPowerConstant = shininess;
			pmc.hasAlphaChannel = hasAlphaChannel ? TRUE : FALSE;

			bindablePtrs.push_back(PixelConstantBuffer<PixelShaderMaterialConstantBuffer>::Resolve(gfx, pmc, 1u));
        }
        else if (hasDiffuseMap && hasNormalMap)
        {
            D3::VertexLayout layout;
            layout.Append(D3::VertexLayout::Position3D)
                .Append(D3::VertexLayout::Normal)
                .Append(D3::VertexLayout::Tangent)
                .Append(D3::VertexLayout::Bitangent)
                .Append(D3::VertexLayout::Texture2D);
            D3::VertexBuffer vertexBuffer(layout);

            for (unsigned int i = 0; i < mesh.mNumVertices; i++)
            {
                vertexBuffer.EmplaceBack(
                    XMFLOAT3(mesh.mVertices[i].x * scale, mesh.mVertices[i].y * scale, mesh.mVertices[i].z * scale),
                    *reinterpret_cast<XMFLOAT3*>(&mesh.mNormals[i]),
                    *reinterpret_cast<XMFLOAT3*>(&mesh.mTangents[i]),
                    *reinterpret_cast<XMFLOAT3*>(&mesh.mBitangents[i]),
                    *reinterpret_cast<XMFLOAT2*>(&mesh.mTextureCoords[0][i])
                );
            }

            std::vector<unsigned short> indices;
            indices.reserve(mesh.mNumFaces * 3);
            for (unsigned int i = 0; i < mesh.mNumFaces; i++)
            {
                const auto& face = mesh.mFaces[i];
                assert(face.mNumIndices == 3);
                indices.push_back(static_cast<unsigned short>(face.mIndices[0]));
                indices.push_back(static_cast<unsigned short>(face.mIndices[1]));
                indices.push_back(static_cast<unsigned short>(face.mIndices[2]));
            }

            bindablePtrs.push_back(VertexBuffer::Resolve(gfx, meshTag, vertexBuffer));
            bindablePtrs.push_back(IndexBuffer::Resolve(gfx, meshTag, indices));

            auto vertexShader = VertexShader::Resolve(gfx, shaderBasePath + "BlinnPhong_NormalMapped_VS.cso");
            auto vertexShaderByteCode = vertexShader->GetByteCode();
            bindablePtrs.push_back(std::move(vertexShader));
            bindablePtrs.push_back(PixelShader::Resolve(gfx, shaderBasePath + "BlinnPhong_NormalMapped_PS.cso"));
            bindablePtrs.push_back(InputLayout::Resolve(gfx, vertexBuffer.GetLayout(), vertexShaderByteCode));

            struct PixelShaderMaterialConstantBuffer
            {
                float specularIntensity = 0.18f;
                float specularPower = 0.0f;
                BOOL normalMapEnabled = TRUE;
                float padding[1] = { 0.0f };
            } pmc;

			pmc.specularPower = shininess;
			pmc.specularIntensity = (meshSpecularColor.x + meshSpecularColor.y + meshSpecularColor.z) / 3.0f;

            bindablePtrs.push_back(PixelConstantBuffer<PixelShaderMaterialConstantBuffer>::Resolve(gfx, pmc, 1u));
        }
        else if (hasDiffuseMap)
        {
			D3::VertexLayout layout;
			layout.Append(D3::VertexLayout::Position3D)
				.Append(D3::VertexLayout::Normal)
				.Append(D3::VertexLayout::Texture2D);
			D3::VertexBuffer vertexBuffer(layout);

            for (unsigned int i = 0; i < mesh.mNumVertices; i++)
            {
                vertexBuffer.EmplaceBack(
                    XMFLOAT3(mesh.mVertices[i].x * scale, mesh.mVertices[i].y * scale, mesh.mVertices[i].z * scale),
                    *reinterpret_cast<XMFLOAT3*>(&mesh.mNormals[i]),
                    *reinterpret_cast<XMFLOAT2*>(&mesh.mTextureCoords[0][i])
				);
            }

			std::vector<unsigned short> indices;
			indices.reserve(mesh.mNumFaces * 3);
            for (unsigned int i = 0; i < mesh.mNumFaces; i++)
            {
				const auto& face = mesh.mFaces[i];
				assert(face.mNumIndices == 3);
				indices.push_back(static_cast<unsigned short>(face.mIndices[0]));
				indices.push_back(static_cast<unsigned short>(face.mIndices[1]));
				indices.push_back(static_cast<unsigned short>(face.mIndices[2]));
            }

			bindablePtrs.push_back(VertexBuffer::Resolve(gfx, meshTag, vertexBuffer));
			bindablePtrs.push_back(IndexBuffer::Resolve(gfx, meshTag, indices));

            auto vertexShader = VertexShader::Resolve(gfx, shaderBasePath + "BlinnPhong_Diffuse_VS.cso");
			auto vertexShaderByteCode = vertexShader->GetByteCode();

			bindablePtrs.push_back(std::move(vertexShader));
			bindablePtrs.push_back(PixelShader::Resolve(gfx, shaderBasePath + "BlinnPhong_Diffuse_PS.cso"));
            bindablePtrs.push_back(InputLayout::Resolve(gfx, vertexBuffer.GetLayout(), vertexShaderByteCode));
            struct PixelShaderMaterialConstantBuffer
            {
                float specularIntensity = 0.18f;
                float specularPower = 0.0f;
                float padding[2] = { 0.0f };
			} pmc;

            pmc.specularPower = shininess;
			pmc.specularIntensity = (meshSpecularColor.x + meshSpecularColor.y + meshSpecularColor.z) / 3.0f;
			bindablePtrs.push_back(PixelConstantBuffer<PixelShaderMaterialConstantBuffer>::Resolve(gfx, pmc, 1u));
        }
        else if (!hasDiffuseMap && !hasNormalMap && !hasSpecularMap)
        {
            D3::VertexLayout layout;
            layout.Append(D3::VertexLayout::Position3D)
                .Append(D3::VertexLayout::Normal);
            D3::VertexBuffer vertexBuffer(layout);
            for (unsigned int i = 0; i < mesh.mNumVertices; i++)
            {
                vertexBuffer.EmplaceBack(
                    XMFLOAT3(mesh.mVertices[i].x * scale, mesh.mVertices[i].y * scale, mesh.mVertices[i].z * scale),
                    *reinterpret_cast<XMFLOAT3*>(&mesh.mNormals[i])
                );
            }
            std::vector<unsigned short> indices;
            indices.reserve(mesh.mNumFaces * 3);
            for (unsigned int i = 0; i < mesh.mNumFaces; i++)
            {
                const auto& face = mesh.mFaces[i];
                assert(face.mNumIndices == 3);
                indices.push_back(static_cast<unsigned short>(face.mIndices[0]));
                indices.push_back(static_cast<unsigned short>(face.mIndices[1]));
                indices.push_back(static_cast<unsigned short>(face.mIndices[2]));
            }
            bindablePtrs.push_back(VertexBuffer::Resolve(gfx, meshTag, vertexBuffer));
            bindablePtrs.push_back(IndexBuffer::Resolve(gfx, meshTag, indices));
            auto vertexShader = VertexShader::Resolve(gfx, shaderBasePath + "BlinnPhong_Solid_VS.cso");
            auto vertexShaderByteCode = vertexShader->GetByteCode();
            bindablePtrs.push_back(std::move(vertexShader));
            bindablePtrs.push_back(PixelShader::Resolve(gfx, shaderBasePath + "BlinnPhong_Solid_PS.cso"));
            bindablePtrs.push_back(InputLayout::Resolve(gfx, vertexBuffer.GetLayout(), vertexShaderByteCode));
            struct PixelShaderMaterialConstantBuffer
            {
                XMFLOAT4 materialColor = { 0.65f, 0.65f, 0.85f, 1.0f };
                float specularIntensity = 0.18f;
                float specularPower = 0.0f;
                float padding[2] = { 0.0f, 0.0f };
            } pmc;
            pmc.specularPower = shininess;
			pmc.specularIntensity = (meshSpecularColor.x + meshSpecularColor.y + meshSpecularColor.z) / 3.0f;
            pmc.materialColor = meshDiffuseColor;
			bindablePtrs.push_back(PixelConstantBuffer<PixelShaderMaterialConstantBuffer>::Resolve(gfx, pmc, 1u));
        }

		bindablePtrs.push_back(Topology::Resolve(gfx, D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST));
    }
    else
    {
		throw std::runtime_error("Mesh has no material.");
    }

    return std::make_unique<Mesh>(gfx, std::move(bindablePtrs));
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


