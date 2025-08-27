#include "Renderable/ModelTest.h"
#include "Geometry/GeometryFactory.h"
#include "Bindable/BindableCommon.h"
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

ModelTest::ModelTest(Graphics& gfx, std::mt19937& rng, std::uniform_real_distribution<float>& adist, std::uniform_real_distribution<float>& ddist, std::uniform_real_distribution<float>& odist, std::uniform_real_distribution<float>& rdist, float scale)
	: RenderableTestObject(rng, adist, ddist, odist, rdist)
{
	Assimp::Importer importer;
	auto pModel = importer.ReadFile("assets\\models\\suzanne.obj",
		aiProcess_Triangulate | aiProcess_JoinIdenticalVertices);

	const auto pMesh = pModel->mMeshes[0];
	std::vector<VertexPositionNormal> vertices;
	vertices.reserve(pMesh->mNumVertices);
	for (unsigned int i = 0; i < pMesh->mNumVertices; i++)
	{
		vertices.push_back(
			{
				{pMesh->mVertices[i].x * scale, pMesh->mVertices[i].y * scale, pMesh->mVertices[i].z * scale},
				*reinterpret_cast<DirectX::XMFLOAT3*>(&pMesh->mNormals[i]) 
			});
	}

	std::vector<unsigned short> indices;
	indices.reserve(pMesh->mNumFaces * 3);
	for (unsigned int i = 0; i < pMesh->mNumFaces; i++)
	{
		const auto& face = pMesh->mFaces[i];
		assert(face.mNumIndices == 3);
		indices.push_back(face.mIndices[0]);
		indices.push_back(face.mIndices[1]);
		indices.push_back(face.mIndices[2]); 
	}

	auto vs = AddSharedBindable<VertexShader>(gfx, "vs_phong", L"shaders/Output/PhongVS.cso");
	const auto shaderByteCode = vs->GetByteCode();
	AddSharedBindable<PixelShader>(gfx, "ps_phong", L"shaders/Output/PhongPS.cso");
	AddSharedBindable<VertexBuffer>(gfx, "suzanne_vb", vertices);
	AddSharedBindable<IndexBuffer>(gfx, "suzzane_ib", indices);

	const std::vector<D3D11_INPUT_ELEMENT_DESC> layout =
	{
		{ "Position", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "Normal", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	};
	AddSharedBindable<InputLayout>(gfx, "position_normal_layout", layout, shaderByteCode);
	AddUniqueBindable(std::make_unique<PixelConstantBuffer<PSMaterialConstantBuffer>>(gfx, materialConstantBuffer, 1u));
	AddSharedBindable<Topology>(gfx, "triangle_list", D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	AddUniqueBindable(std::make_unique<TransformConstantBuffer>(gfx, *this));

}

DirectX::XMMATRIX ModelTest::GetTransformXM() const noexcept
{
	return DirectX::XMMatrixRotationRollPitchYaw(pitch, yaw, roll) *
		DirectX::XMMatrixTranslation(r, 0.0f, 0.0f) *
		DirectX::XMMatrixRotationRollPitchYaw(theta, phi, chi);
}
