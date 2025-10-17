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
		Material(Graphics& gfx, const aiMaterial* pMaterial, const std::filesystem::path& path);
		
		D3::VertexBuffer ExtractVertices(const aiMesh& mesh) const noexcept;
		std::vector<Technique> GetTechniques() const noexcept;

	private:
		D3::VertexLayout vertexLayout;
		std::vector<Technique> techniques;
	};
}

