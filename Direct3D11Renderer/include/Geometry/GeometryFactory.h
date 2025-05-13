#pragma once
#define _USE_MATH_DEFINES
#include "GeometryMesh.h"
#include <cmath>
#include <vector>

class GeometryFactory
{
public:
	template<typename VertexType>
	static GeometryMesh<VertexType> CreateCube(float size = 1.0f)
	{
		const float halfSize = size / 2.0f;

        // Define the 8 corners of the cube
        std::vector<DirectX::XMFLOAT3> positions = {
            { -halfSize, -halfSize, -halfSize }, // 0: bottom-left-back
            {  halfSize, -halfSize, -halfSize }, // 1: bottom-right-back
            { -halfSize,  halfSize, -halfSize }, // 2: top-left-back
            {  halfSize,  halfSize, -halfSize }, // 3: top-right-back
            { -halfSize, -halfSize,  halfSize }, // 4: bottom-left-front
            {  halfSize, -halfSize,  halfSize }, // 5: bottom-right-front
            { -halfSize,  halfSize,  halfSize }, // 6: top-left-front
            {  halfSize,  halfSize,  halfSize }  // 7: top-right-front
        };

        // Create vertices from positions
        std::vector<VertexType> vertices(positions.size());
        for (size_t i = 0; i < positions.size(); i++)
        {
            vertices[i].position = positions[i];
        }

        // Define the 12 triangles (6 faces * 2 triangles per face)
        std::vector<unsigned short> indices = {
            0, 2, 1,  2, 3, 1,  // Back face
            1, 3, 5,  3, 7, 5,  // Right face
            2, 6, 3,  3, 6, 7,  // Top face
            4, 5, 7,  4, 7, 6,  // Front face
            0, 4, 2,  2, 4, 6,  // Left face
            0, 1, 4,  1, 5, 4   // Bottom face
        };

        return GeometryMesh<VertexType>(std::move(vertices), std::move(indices));
	}

    // Create a sphere centered at the origin with specified tessellation
    //template<typename VertexType>
    //static GeometryMesh<VertexType> CreateSphere(float radius = 1.0f, int tessellation = 16)
    //{
    //    // Ensure minimum tessellation
    //    tessellation = std::max(3, tessellation);

    //    std::vector<VertexType> vertices;
    //    std::vector<unsigned short> indices;

    //    // Create rings of vertices at different heights
    //    for (int i = 0; i <= tessellation; i++)
    //    {
    //        const float v = 1.0f - (float)i / tessellation; // [1, 0]
    //        const float phi = v * PI; // [PI, 0]

    //        // Create the current ring
    //        for (int j = 0; j <= tessellation; j++)
    //        {
    //            const float u = (float)j / tessellation; // [0, 1]
    //            const float theta = u * PI * 2; // [0, 2PI]

    //            // Calculate position
    //            const float x = radius * std::sin(phi) * std::cos(theta);
    //            const float y = radius * std::cos(phi);
    //            const float z = radius * std::sin(phi) * std::sin(theta);

    //            VertexType vertex;
    //            vertex.pos = { x, y, z };
    //            vertices.push_back(vertex);
    //        }
    //    }

    //    // Create indices for triangles
    //    const int stride = tessellation + 1;
    //    for (int i = 0; i < tessellation; i++)
    //    {
    //        for (int j = 0; j < tessellation; j++)
    //        {
    //            const int topLeft = i * stride + j;
    //            const int topRight = topLeft + 1;
    //            const int bottomLeft = (i + 1) * stride + j;
    //            const int bottomRight = bottomLeft + 1;

    //            // First triangle (top-left, bottom-left, bottom-right)
    //            indices.push_back(topLeft);
    //            indices.push_back(bottomLeft);
    //            indices.push_back(bottomRight);

    //            // Second triangle (top-left, bottom-right, top-right)
    //            indices.push_back(topLeft);
    //            indices.push_back(bottomRight);
    //            indices.push_back(topRight);
    //        }
    //    }

    //    return Mesh<VertexType>(std::move(vertices), std::move(indices));
    //}
};