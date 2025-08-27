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
/// A node represents a singular node encompassing zero to many meshes within a scene graph.
/// A node potentially has child nodes which each of those could have their own children. 
/// You build out a scene by constructing a root node, and filling children nodes recursively.
/// </summary>
class Node
{
public:
    Node(std::vector<Mesh*> meshes, const DirectX::XMMATRIX& transform);
    void Draw(Graphics& gfx, DirectX::FXMMATRIX parentTransform) const noexcept;
    void AddChild(std::unique_ptr<Node> child) noexcept;
private:
    std::vector<std::unique_ptr<Node>> children;
    std::vector<Mesh*> meshes;
    DirectX::XMFLOAT4X4 localTransform{};
};

class Model
{
public:
    Model(Graphics& gfx, const std::string& filePath);
    void Draw(Graphics& gfx, DirectX::FXMMATRIX world) const noexcept;
private:
    std::unique_ptr<Mesh> BuildMesh(Graphics& gfx, const aiMesh& mesh);
    std::unique_ptr<Node> BuildNode(const aiNode& node);
private:
    std::unique_ptr<Node> root;
    std::vector<std::unique_ptr<Mesh>> meshes;
};


