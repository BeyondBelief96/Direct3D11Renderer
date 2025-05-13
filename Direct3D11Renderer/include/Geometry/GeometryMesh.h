#pragma once

#include <vector>
#include <DirectXMath.h>
#include <cassert>

struct VertexPosition
{
	DirectX::XMFLOAT3 position;
};

template<typename VertexType>
class GeometryMesh
{
public:
	GeometryMesh() = default;
	GeometryMesh(std::vector<VertexType> vertices_in, std::vector<unsigned short> indicies_in)
		: vertices(std::move(vertices_in)), indices(std::move(indicies_in))
	{
		assert(!this->vertices.empty() && "Mesh must have at least one vertex.");
		assert(this->vertices.size() > 2 && "Mesh must have at least 3 vertices.");
		assert(!this->indices.empty() && "Mesh must have at least one index.");
		assert(this->indices.size() % 3 == 0 && "Mesh indices must be a multiple of 3.");
	}

	void Transform(DirectX::FXMMATRIX matrix)
	{
		for (auto& vertex : vertices)
		{
			const DirectX::XMVECTOR position = DirectX::XMLoadFloat3(&vertex.position);
			DirectX::XMStoreFloat3(&vertex.position, DirectX::XMVector3Transform(position, matrix));
		}
	}

	std::vector<VertexType> vertices;
	std::vector<unsigned short> indices;
};
