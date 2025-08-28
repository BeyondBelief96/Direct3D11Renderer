#pragma once

#include "Renderable/Model/Mesh.h"
#include "Graphics/Graphics.h"
#include <DirectXMath.h>
#include <assimp/scene.h>
#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>
#include <memory>
#include <vector>
#include <string>

/// <summary>
/// A node in the model's scene graph, containing meshes and child nodes.
/// Each node has a local transformation matrix.
/// The Render method applies the node's transformation and renders its meshes and child nodes recursively.
/// 
/// Note: This class is intended for internal use by the Model class only.
/// It is not designed for public use or extension.
/// </summary>
class Node
{
public:
    Node(const std::string& name, std::vector<Mesh*> meshes, const DirectX::XMMATRIX& transform);
    void Render(Graphics& gfx, DirectX::FXMMATRIX parentTransform) const noexcept;
    void RenderTree() const noexcept;
    void AddChild(std::unique_ptr<Node> child) noexcept;
private:
    std::string name;
    std::vector<std::unique_ptr<Node>> children;
    std::vector<Mesh*> meshes;
    DirectX::XMFLOAT4X4 localTransform{};
};

/// <summary>
/// A 3D model composed of meshes and organized in a scene graph of nodes.
/// Loads model data from a file using Assimp and constructs the scene graph.
/// Provides functionality to render the model and display a control window for debugging.
/// </summary>
class Model
{
public:
    Model(Graphics& gfx, const std::string& filePath);
    void Render(Graphics& gfx) const noexcept;
    void ShowModelControlWindow(const char* windowName = nullptr) noexcept;
private:
    std::unique_ptr<Mesh> BuildMesh(Graphics& gfx, const aiMesh& mesh);
    std::unique_ptr<Node> BuildNode(const aiNode& node) noexcept;
private:
    std::unique_ptr<Node> root;
    std::vector<std::unique_ptr<Mesh>> meshes;
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


