#pragma once

#include <vector>
#include <DirectXMath.h>
#include <cassert>

struct VertexPosition
{
	DirectX::XMFLOAT3 position;
};

struct VertexPositionTexture
{
	DirectX::XMFLOAT3 position;
	DirectX::XMFLOAT2 texCoord;
};

struct VertexPositionNormalTexture
{
	DirectX::XMFLOAT3 position;
	DirectX::XMFLOAT3 normal;
	DirectX::XMFLOAT2 texCoord;
};

// Helper type traits to check for vertex members
template <typename T, typename = void>
struct has_texcoord_member : std::false_type {};

template <typename T>
struct has_texcoord_member<T,
    std::void_t<decltype(std::declval<T>().texCoord)>>
    : std::true_type {};

// Helper to check if a vertex type has a normal member
template <typename T, typename = void>
struct has_normal_member : std::false_type {};

template <typename T>
struct has_normal_member<T,
    std::void_t<decltype(std::declval<T>().normal)>>
    : std::true_type {};

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

    void SetFlatNormals() noexcept
    {
        if constexpr (has_normal_member<VertexType>::value)
        {
            using namespace DirectX;
            assert(indices.size() % 3 == 0 && indices.size() > 0);

            for (size_t i = 0; i < indices.size(); i += 3)
            {
                auto& v0 = vertices[indices[i]];
                auto& v1 = vertices[indices[i + 1]];
                auto& v2 = vertices[indices[i + 2]];

                const auto p0 = XMLoadFloat3(&v0.position);
                const auto p1 = XMLoadFloat3(&v1.position);
                const auto p2 = XMLoadFloat3(&v2.position);

                // Calculate normal for this face
                const auto n = XMVector3Normalize(XMVector3Cross((p1 - p0), (p2 - p0)));

                // Store the same normal for all three vertices
                XMFLOAT3 normalFloat;
                XMStoreFloat3(&normalFloat, n);

                v0.normal = normalFloat;
                v1.normal = normalFloat;
                v2.normal = normalFloat;
            }
        }
    }

	std::vector<VertexType> vertices;
	std::vector<unsigned short> indices;
};
