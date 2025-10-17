#include "Renderable/Material/Material.h"

namespace D3
{
	Material::Material(Graphics& gfx, const aiMaterial* pMaterial, const std::filesystem::path& path)
	{
		
	}

	D3::VertexBuffer Material::ExtractVertices(const aiMesh& mesh) const noexcept
	{
		D3::VertexBuffer buffer{ vertexLayout };
		buffer.Resize(mesh.mNumVertices);
		if (vertexLayout.Has<D3::VertexLayout::ElementType::Position3D>())
		{
			for (int i = 0; i < mesh.mNumVertices; i++)
			{
				buffer[i];
			}
		}

		return buffer;
	}

	std::vector<Technique> Material::GetTechniques() const noexcept
	{
		return techniques;
	}
}