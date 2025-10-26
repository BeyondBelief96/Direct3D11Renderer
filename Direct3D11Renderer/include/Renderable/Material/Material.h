#pragma once

#include <material.h>
#include <mesh.h>
#include "Bindable/BindableCommon.h"
#include "RenderPass/Technique.h"
#include <vector>
#include <filesystem>


namespace D3
{
	class Material
	{
	public:
		Material(Graphics& gfx, const aiMaterial& material, const std::filesystem::path& modelPath) noexcept;
		D3::VertexBuffer ExtractVertices(const aiMesh& mesh) const noexcept;
		std::vector<unsigned short> ExtractIndices(const aiMesh& mesh) const noexcept;
		std::vector<Technique> GetTechniques() const noexcept;
		std::shared_ptr<::VertexBuffer> MakeVertexBufferBindable(Graphics& gfx, const aiMesh& mesh) const noexcept;
		std::shared_ptr<::IndexBuffer> MakeIndexBufferBindable(Graphics& gfx, const aiMesh& mesh) const noexcept;
		std::vector<Technique> GetTechniques() noexcept;
	private:
		std::string MakeMeshTag(const aiMesh& mesh) const noexcept;
		D3::VertexLayout vertexLayout;
		std::vector<Technique> techniques;
		std::string modelPath;
		std::string name;
	};
}

