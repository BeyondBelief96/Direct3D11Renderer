#include "Renderable/Material/Material.h"
#include "DynamicConstantBuffer/DynamicConstantBuffer.h"
#include "Bindable/DynamicConstantBufferBindable.h"

namespace D3
{
	Material::Material(Graphics& gfx, const aiMaterial& material, const std::filesystem::path& modelPath) noexcept
		: modelPath(modelPath.string())
	{
		const auto rootPath = modelPath.parent_path().string() + "\\";
		{
			aiString tempName;
			material.Get(AI_MATKEY_NAME, tempName);
			name = tempName.C_Str();
		}
		// phong technique
		{
			Technique phong("Phong");
			Step step(0);
			std::string shaderCode = "Phong";
			aiString textureFileName;

			// Common
			vertexLayout.Append(D3::VertexLayout::ElementType::Position3D);
			vertexLayout.Append(D3::VertexLayout::ElementType::Normal);
			LayoutBuilder pscLayout;
			bool hasTexture = false;
			bool hasGlossAlpha = false;

			// Diffuse
			{
				bool hasAlpha = false;
				if (material.GetTexture(aiTextureType_DIFFUSE, 0, &textureFileName) == aiReturn_SUCCESS)
				{
					hasTexture = true;
					shaderCode += "Diff";
					vertexLayout.Append(D3::VertexLayout::ElementType::Texture2D);
					auto tex = Texture::Resolve(gfx, rootPath + textureFileName.C_Str());
					if (tex->AlphaChannelLoaded())
					{
						hasAlpha = true;
						shaderCode += "Msk";
					}
					step.AddBindable(std::move(tex));
				}
				else
				{
					pscLayout.Add<D3::ElementType::Float3>("materialColor");
				}
				step.AddBindable(Rasterizer::Resolve(gfx, hasAlpha));
			}
			// Specular
			{
				if (material.GetTexture(aiTextureType_SPECULAR, 0, &textureFileName) == aiReturn_SUCCESS)
				{
					hasTexture = true;
					shaderCode += "Spc";
					vertexLayout.Append(D3::VertexLayout::ElementType::Texture2D);
					auto tex = Texture::Resolve(gfx, rootPath + textureFileName.C_Str(), 1);
					hasGlossAlpha = tex->AlphaChannelLoaded();
					step.AddBindable(std::move(tex));
					pscLayout.Add<D3::ElementType::Bool>("useGlassAlpha");
				}
				pscLayout.Add<D3::ElementType::Float3>("specularColor");
				pscLayout.Add<D3::ElementType::Float>("specularWeight");
				pscLayout.Add<D3::ElementType::Float>("specularGloss");
			}
			// Normal
			{
				if (material.GetTexture(aiTextureType_NORMALS, 0, &textureFileName) == aiReturn_SUCCESS)
				{
					hasTexture = true;
					shaderCode += "Nrm";
					vertexLayout.Append(D3::VertexLayout::ElementType::Texture2D);
					vertexLayout.Append(D3::VertexLayout::ElementType::Tangent);
					vertexLayout.Append(D3::VertexLayout::ElementType::Bitangent);
					auto tex = Texture::Resolve(gfx, rootPath + textureFileName.C_Str(), 2);
					step.AddBindable(std::move(tex));
					pscLayout.Add<D3::ElementType::Bool>("useNormalMap");
					pscLayout.Add<D3::ElementType::Float>("normalMapWeight");
				};
			}
			// common (post)
			{
				step.AddBindable(std::make_shared<TransformConstantBuffer>(gfx, 0u));
				step.AddBindable(Blender::Resolve(gfx, false));
				auto pvs = VertexShader::Resolve(gfx, shaderCode + "VS.cso");
				auto pvsbc = pvs->GetByteCode();
				step.AddBindable(std::move(pvs));
				step.AddBindable(PixelShader::Resolve(gfx, shaderCode + "PS.cso"));
				step.AddBindable(InputLayout::Resolve(gfx, vertexLayout, pvsbc));
				if (hasTexture)
				{
					step.AddBindable(Sampler::Resolve(gfx));
				}
				// PS Material params (constant buffer)
				D3::ConstantBufferData buffer{ std::move(pscLayout) };
				if (auto r = buffer["materialColor"]; r.Exists())
				{
					aiColor3D color = { 0.45f, 0.45f, 0.85f };
					material.Get(AI_MATKEY_COLOR_DIFFUSE, color);
					r = reinterpret_cast<DirectX::XMFLOAT3&>(color);
				}
				buffer["useGlossAlpha"].TrySet(hasGlossAlpha);
				if (auto r = buffer["specularColor"]; r.Exists())
				{
					aiColor3D color = { 0.18f,0.18f,0.18f };
					material.Get(AI_MATKEY_COLOR_SPECULAR, color);
					r = reinterpret_cast<DirectX::XMFLOAT3&>(color);
				}
				buffer["specularWeight"].TrySet(1.0f);
				if (auto r = buffer["specularGloss"]; r.Exists())
				{
					float gloss = 8.0f;
					material.Get(AI_MATKEY_SHININESS, gloss);
					r = gloss;
				}
				buffer["useNormalMap"].TrySet(true);
				buffer["normalMapWeight"].TrySet(1.0f);
				step.AddBindable(std::make_unique<CachingDynamicPixelConstantBufferBindable>(gfx, std::move(buffer), 1u));
			}
			phong.AddStep(std::move(step));
			techniques.push_back(std::move(phong));
		}
		// Outline technique
		{

		}
	}

	D3::VertexBuffer Material::ExtractVertices(const aiMesh& mesh) const noexcept
	{
		return VertexBuffer{ vertexLayout, mesh };
	}

	std::vector<unsigned short> Material::ExtractIndices(const aiMesh& mesh) const noexcept
	{
		std::vector<unsigned short> indices;
		indices.reserve(mesh.mNumFaces * 3);
		for (unsigned int i = 0; i < mesh.mNumFaces; i++)
		{
			const auto& face = mesh.mFaces[i];
			assert(face.mNumIndices == 3);
			indices.push_back(face.mIndices[0]);
			indices.push_back(face.mIndices[1]);
			indices.push_back(face.mIndices[2]);
		}
		return indices;
	}

	std::vector<Technique> Material::GetTechniques() const noexcept
	{
		return techniques;
	}

	std::shared_ptr<::VertexBuffer> Material::MakeVertexBufferBindable(Graphics& gfx, const aiMesh& mesh) const noexcept
	{
		return ::VertexBuffer::Resolve(gfx, MakeMeshTag(mesh), ExtractVertices(mesh));
	}

	std::shared_ptr<::IndexBuffer> Material::MakeIndexBufferBindable(Graphics& gfx, const aiMesh& mesh) const noexcept
	{
		return ::IndexBuffer::Resolve(gfx, MakeMeshTag(mesh), ExtractIndices(mesh));
	}

	std::string Material::MakeMeshTag(const aiMesh& mesh) const noexcept
	{
		return modelPath + "%" + mesh.mName.C_Str();
	}
}