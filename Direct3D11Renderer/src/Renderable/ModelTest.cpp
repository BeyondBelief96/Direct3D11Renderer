#include "Renderable/ModelTest.h"
#include "Geometry/GeometryFactory.h"
#include "Bindable/BindableCommon.h"
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include "Geometry/Vertex.h"

ModelTest::ModelTest(Graphics& gfx, std::mt19937& rng, std::uniform_real_distribution<float>& adist, std::uniform_real_distribution<float>& ddist, std::uniform_real_distribution<float>& odist, std::uniform_real_distribution<float>& rdist, float scale)
	: RenderableTestObject(rng, adist, ddist, odist, rdist)
{
	Assimp::Importer importer;
	auto pModel = importer.ReadFile("assets\\models\\suzanne.obj",
		aiProcess_Triangulate | aiProcess_JoinIdenticalVertices);

	const auto pMesh = pModel->mMeshes[0];
	// Build dynamic vertex buffer with layout Position3D + Normal
	D3::VertexLayout dynLayout;
	dynLayout.Append(D3::VertexLayout::Position3D)
			 .Append(D3::VertexLayout::Normal);
	D3::VertexBuffer dynVbuf(std::move(dynLayout));
	for (unsigned int i = 0; i < pMesh->mNumVertices; i++)
	{
		dynVbuf.EmplaceBack(
			DirectX::XMFLOAT3{ pMesh->mVertices[i].x * scale, pMesh->mVertices[i].y * scale, pMesh->mVertices[i].z * scale },
			*reinterpret_cast<DirectX::XMFLOAT3*>(&pMesh->mNormals[i])
		);
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
	// Upload dynamic vertex buffer to GPU using CPU dynamic buffer overload
	AddSharedBindable<VertexBuffer>(gfx, "suzanne_vb", dynVbuf);
	AddSharedBindable<IndexBuffer>(gfx, "suzzane_ib", indices);

	// Create InputLayout from dynamic layout
	AddSharedBindable<InputLayout>(gfx, "position_normal_layout", dynVbuf.GetLayout().GetD3DLayout(), shaderByteCode);
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
