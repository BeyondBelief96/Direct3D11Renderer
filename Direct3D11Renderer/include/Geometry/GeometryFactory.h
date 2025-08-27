#pragma once
#include "GeometryMesh.h"
#include <cmath>
#include <vector>
#include <array>

constexpr float PI = 3.14159265f;
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

        std::vector<unsigned short> indices = {
            0, 2, 1,  2, 3, 1,  // Front face (-Z) - closer to viewer
            1, 3, 5,  3, 7, 5,  // Right face (+X)
            2, 6, 3,  3, 6, 7,  // Top face (+Y)
            4, 5, 7,  4, 7, 6,  // Back face (+Z) - further from viewer
            0, 4, 2,  2, 4, 6,  // Left face (-X)
            0, 1, 4,  1, 5, 4   // Bottom face (-Y)
        };

        return GeometryMesh<VertexType>(std::move(vertices), std::move(indices));
	}

    template<typename VertexType>
    static GeometryMesh<VertexType> CreateIndependentCube(float size = 1.0f)
    {
        const float halfSize = size / 2.0f;

        // Create 24 vertices (4 for each of the 6 faces)
        std::vector<VertexType> vertices(24);

        // Near side (negative Z)
        vertices[0].position = { -halfSize, -halfSize, -halfSize }; // bottom-left
        vertices[1].position = { halfSize, -halfSize, -halfSize };  // bottom-right
        vertices[2].position = { -halfSize, halfSize, -halfSize };  // top-left
        vertices[3].position = { halfSize, halfSize, -halfSize };   // top-right

        // Far side (positive Z)
        vertices[4].position = { -halfSize, -halfSize, halfSize };  // bottom-left
        vertices[5].position = { halfSize, -halfSize, halfSize };   // bottom-right
        vertices[6].position = { -halfSize, halfSize, halfSize };   // top-left
        vertices[7].position = { halfSize, halfSize, halfSize };    // top-right

        // Left side (negative X)
        vertices[8].position = { -halfSize, -halfSize, -halfSize }; // bottom-near
        vertices[9].position = { -halfSize, halfSize, -halfSize };  // top-near
        vertices[10].position = { -halfSize, -halfSize, halfSize }; // bottom-far
        vertices[11].position = { -halfSize, halfSize, halfSize };  // top-far

        // Right side (positive X)
        vertices[12].position = { halfSize, -halfSize, -halfSize }; // bottom-near
        vertices[13].position = { halfSize, halfSize, -halfSize };  // top-near
        vertices[14].position = { halfSize, -halfSize, halfSize };  // bottom-far
        vertices[15].position = { halfSize, halfSize, halfSize };   // top-far

        // Bottom side (negative Y)
        vertices[16].position = { -halfSize, -halfSize, -halfSize }; // left-near
        vertices[17].position = { halfSize, -halfSize, -halfSize };  // right-near
        vertices[18].position = { -halfSize, -halfSize, halfSize };  // left-far
        vertices[19].position = { halfSize, -halfSize, halfSize };   // right-far

        // Top side (positive Y)
        vertices[20].position = { -halfSize, halfSize, -halfSize }; // left-near
        vertices[21].position = { halfSize, halfSize, -halfSize };  // right-near
        vertices[22].position = { -halfSize, halfSize, halfSize };  // left-far
        vertices[23].position = { halfSize, halfSize, halfSize };   // right-far

        // Add per-face normals if the vertex type supports it
        if constexpr (has_normal_member<VertexType>::value) {
            // Near face (negative Z)
            for (int i = 0; i < 4; i++) {
                vertices[i].normal = { 0.0f, 0.0f, -1.0f };
            }

            // Far face (positive Z)
            for (int i = 4; i < 8; i++) {
                vertices[i].normal = { 0.0f, 0.0f, 1.0f };
            }

            // Left face (negative X)
            for (int i = 8; i < 12; i++) {
                vertices[i].normal = { -1.0f, 0.0f, 0.0f };
            }

            // Right face (positive X)
            for (int i = 12; i < 16; i++) {
                vertices[i].normal = { 1.0f, 0.0f, 0.0f };
            }

            // Bottom face (negative Y)
            for (int i = 16; i < 20; i++) {
                vertices[i].normal = { 0.0f, -1.0f, 0.0f };
            }

            // Top face (positive Y)
            for (int i = 20; i < 24; i++) {
                vertices[i].normal = { 0.0f, 1.0f, 0.0f };
            }
        }

        // Add texture coordinates if the vertex type supports them
        if constexpr (has_texcoord_member<VertexType>::value) {
            // For each face, define texture coordinates
            // Bottom-left, bottom-right, top-left, top-right
            const DirectX::XMFLOAT2 texCoords[4] = {
                { 0.0f, 1.0f }, // Bottom-left
                { 1.0f, 1.0f }, // Bottom-right
                { 0.0f, 0.0f }, // Top-left
                { 1.0f, 0.0f }  // Top-right
            };

            // Apply the texture coordinates to each face
            for (int face = 0; face < 6; face++) {
                for (int i = 0; i < 4; i++) {
                    vertices[face * 4 + i].texCoord = texCoords[i];
                }
            }
        }

        // Define indices for all faces (2 triangles per face)
        std::vector<unsigned short> indices = {
            // Near face (negative Z)
            0, 2, 1,    2, 3, 1,
            // Far face (positive Z)
            4, 5, 7,    4, 7, 6,
            // Left face (negative X)
            8, 10, 9,   10, 11, 9,
            // Right face (positive X)
            12, 13, 15, 12, 15, 14,
            // Bottom face (negative Y)
            16, 17, 18, 18, 17, 19,
            // Top face (positive Y)
            20, 23, 21, 20, 22, 23
        };

        return GeometryMesh<VertexType>(std::move(vertices), std::move(indices));
    }

    template<typename VertexType>
    static GeometryMesh<VertexType> CreateTexturedCube(float size = 1.0f)
    {
        const float halfSize = size / 2.0f;

        // Create vertices with both position and texture coordinates
        std::vector<VertexType> vertices(14);

        // Front face vertices
        vertices[0].position = { -halfSize, -halfSize, -halfSize }; // bottom-left
        vertices[0].texCoord = { 2.0f / 3.0f, 0.0f / 4.0f };

        vertices[1].position = { halfSize, -halfSize, -halfSize }; // bottom-right
        vertices[1].texCoord = { 1.0f / 3.0f, 0.0f / 4.0f };

        vertices[2].position = { -halfSize, halfSize, -halfSize }; // top-left
        vertices[2].texCoord = { 2.0f / 3.0f, 1.0f / 4.0f };

        vertices[3].position = { halfSize, halfSize, -halfSize }; // top-right
        vertices[3].texCoord = { 1.0f / 3.0f, 1.0f / 4.0f };

        // Back face vertices
        vertices[4].position = { -halfSize, -halfSize, halfSize }; // bottom-left
        vertices[4].texCoord = { 2.0f / 3.0f, 3.0f / 4.0f };

        vertices[5].position = { halfSize, -halfSize, halfSize }; // bottom-right
        vertices[5].texCoord = { 1.0f / 3.0f, 3.0f / 4.0f };

        vertices[6].position = { -halfSize, halfSize, halfSize }; // top-left
        vertices[6].texCoord = { 2.0f / 3.0f, 2.0f / 4.0f };

        vertices[7].position = { halfSize, halfSize, halfSize }; // top-right
        vertices[7].texCoord = { 1.0f / 3.0f, 2.0f / 4.0f };

        // Additional vertices for bottom face
        vertices[8].position = { -halfSize, -halfSize, -halfSize }; // reused position
        vertices[8].texCoord = { 2.0f / 3.0f, 4.0f / 4.0f };

        vertices[9].position = { halfSize, -halfSize, -halfSize }; // reused position
        vertices[9].texCoord = { 1.0f / 3.0f, 4.0f / 4.0f };

        // Additional vertices for left face
        vertices[10].position = { -halfSize, -halfSize, -halfSize }; // reused position
        vertices[10].texCoord = { 3.0f / 3.0f, 1.0f / 4.0f };

        vertices[11].position = { -halfSize, -halfSize, halfSize }; // reused position
        vertices[11].texCoord = { 3.0f / 3.0f, 2.0f / 4.0f };

        // Additional vertices for right face
        vertices[12].position = { halfSize, -halfSize, -halfSize }; // reused position
        vertices[12].texCoord = { 0.0f / 3.0f, 1.0f / 4.0f };

        vertices[13].position = { halfSize, -halfSize, halfSize }; // reused position
        vertices[13].texCoord = { 0.0f / 3.0f, 2.0f / 4.0f };

        // Define indices for each face
        std::vector<unsigned short> indices = {
            0,2,1,    2,3,1,    // Front face
            4,8,5,    5,8,9,    // Bottom face (reusing some vertices with different UVs)
            2,6,3,    3,6,7,    // Top face
            4,5,7,    4,7,6,    // Back face
            2,10,11,  2,11,6,   // Left face (reusing positions but with different UVs)
            12,3,7,   12,7,13   // Right face (reusing positions but with different UVs)
        };

        return GeometryMesh<VertexType>(std::move(vertices), std::move(indices));
    }

    template<typename VertexType>
    static GeometryMesh<VertexType> CreateIndependentTexturedCube(float size = 1.0f)
    {
        static_assert(has_texcoord_member<VertexType>::value, "VertexType must have texCoord member for textured cube");

        const float halfSize = size / 2.0f;

        // Create 24 vertices (4 for each of the 6 faces)
        std::vector<VertexType> vertices(24);

        // Near side (negative Z)
        vertices[0].position = { -halfSize, -halfSize, -halfSize }; // bottom-left
        vertices[1].position = { halfSize, -halfSize, -halfSize };  // bottom-right
        vertices[2].position = { -halfSize, halfSize, -halfSize };  // top-left
        vertices[3].position = { halfSize, halfSize, -halfSize };   // top-right

        // Far side (positive Z)
        vertices[4].position = { -halfSize, -halfSize, halfSize };  // bottom-left
        vertices[5].position = { halfSize, -halfSize, halfSize };   // bottom-right
        vertices[6].position = { -halfSize, halfSize, halfSize };   // top-left
        vertices[7].position = { halfSize, halfSize, halfSize };    // top-right

        // Left side (negative X)
        vertices[8].position = { -halfSize, -halfSize, -halfSize }; // bottom-near
        vertices[9].position = { -halfSize, halfSize, -halfSize };  // top-near
        vertices[10].position = { -halfSize, -halfSize, halfSize }; // bottom-far
        vertices[11].position = { -halfSize, halfSize, halfSize };  // top-far

        // Right side (positive X)
        vertices[12].position = { halfSize, -halfSize, -halfSize }; // bottom-near
        vertices[13].position = { halfSize, halfSize, -halfSize };  // top-near
        vertices[14].position = { halfSize, -halfSize, halfSize };  // bottom-far
        vertices[15].position = { halfSize, halfSize, halfSize };   // top-far

        // Bottom side (negative Y)
        vertices[16].position = { -halfSize, -halfSize, -halfSize }; // left-near
        vertices[17].position = { halfSize, -halfSize, -halfSize };  // right-near
        vertices[18].position = { -halfSize, -halfSize, halfSize };  // left-far
        vertices[19].position = { halfSize, -halfSize, halfSize };   // right-far

        // Top side (positive Y)
        vertices[20].position = { -halfSize, halfSize, -halfSize }; // left-near
        vertices[21].position = { halfSize, halfSize, -halfSize };  // right-near
        vertices[22].position = { -halfSize, halfSize, halfSize };  // left-far
        vertices[23].position = { halfSize, halfSize, halfSize };   // right-far

        // Add texture coordinates for each face
        // Each face gets a standard set of coordinates:
        // (0,1) bottom-left, (1,1) bottom-right, (0,0) top-left, (1,0) top-right
        const DirectX::XMFLOAT2 texCoords[4] = {
            { 0.0f, 1.0f }, // Bottom-left
            { 1.0f, 1.0f }, // Bottom-right
            { 0.0f, 0.0f }, // Top-left
            { 1.0f, 0.0f }  // Top-right
        };

        // Apply the texture coordinates to each face
        for (int face = 0; face < 6; face++) {
            for (int i = 0; i < 4; i++) {
                vertices[face * 4 + i].texCoord = texCoords[i];
            }
        }

        // Add normals if the vertex type supports it
        if constexpr (has_normal_member<VertexType>::value) {
            // Near face (negative Z)
            for (int i = 0; i < 4; i++) {
                vertices[i].normal = { 0.0f, 0.0f, -1.0f };
            }

            // Far face (positive Z)
            for (int i = 4; i < 8; i++) {
                vertices[i].normal = { 0.0f, 0.0f, 1.0f };
            }

            // Left face (negative X)
            for (int i = 8; i < 12; i++) {
                vertices[i].normal = { -1.0f, 0.0f, 0.0f };
            }

            // Right face (positive X)
            for (int i = 12; i < 16; i++) {
                vertices[i].normal = { 1.0f, 0.0f, 0.0f };
            }

            // Bottom face (negative Y)
            for (int i = 16; i < 20; i++) {
                vertices[i].normal = { 0.0f, -1.0f, 0.0f };
            }

            // Top face (positive Y)
            for (int i = 20; i < 24; i++) {
                vertices[i].normal = { 0.0f, 1.0f, 0.0f };
            }
        }

        // Define indices for all faces (2 triangles per face)
        std::vector<unsigned short> indices = {
            // Near face (negative Z)
            0, 2, 1,    2, 3, 1,
            // Far face (positive Z)
            4, 5, 7,    4, 7, 6,  
            // Left face (negative X)
            8, 10, 9,   9, 10, 11,
            // Right face (positive X)
            12, 13, 15, 12, 15, 14,
            // Bottom face (negative Y)
            16, 17, 18, 18, 17, 19,
            // Top face (positive Y)
            20, 23, 21, 20, 22, 23
        };

        return GeometryMesh<VertexType>(std::move(vertices), std::move(indices));
    }

    template<typename VertexType>
    static GeometryMesh<VertexType> CreateSphere(float radius = 1.0f, int latDiv = 12, int longDiv = 24)
    {
        namespace dx = DirectX;

        // Ensure minimum divisions
        latDiv = std::max(3, latDiv);
        longDiv = std::max(3, longDiv);

        std::vector<VertexType> vertices;

        // Create rings of vertices around the sphere (excluding poles)
        for (int iLat = 0; iLat <= latDiv; iLat++)
        {
            // Calculate the angle from the pole for this latitude ring
            const float phi = (PI * iLat) / latDiv;

            // Calculate coordinates for this ring
            const float y = radius * std::cos(phi);
            const float ringRadius = radius * std::sin(phi);

            for (int iLong = 0; iLong < longDiv; iLong++)
            {
                // Calculate angle around the sphere for this longitude
                const float theta = (2.0f * PI * iLong) / longDiv;

                // Calculate vertex position
                const float x = ringRadius * std::cos(theta);
                const float z = ringRadius * std::sin(theta);

                VertexType vertex;
                vertex.position = { x, y, z };

                // Add any other vertex attributes here as needed
                // For example, calculate normals by normalizing the position
                if constexpr (has_normal_member<VertexType>::value) {
                    // Normalize position to get normal - this ensures correct outward-facing normals
                    float len = radius; // Since we're creating a sphere, the length should always be the radius
                    vertex.normal = { x / len, y / len, z / len };
                }

                vertices.push_back(vertex);
            }
        }

        std::vector<unsigned short> indices;

        // Create indices for the main body of the sphere (connecting latitude rings)
        for (unsigned short iLat = 0; iLat < latDiv; iLat++)
        {
            for (unsigned short iLong = 0; iLong < longDiv; iLong++)
            {
                // Calculate indices for the two triangles of each quad
                unsigned short i1 = iLat * longDiv + iLong;
                unsigned short i2 = i1 + longDiv;
                unsigned short i3 = (iLat * longDiv) + ((iLong + 1) % longDiv);
                unsigned short i4 = i3 + longDiv;

                // First triangle
                indices.push_back(i1);
                indices.push_back(i3);
                indices.push_back(i2);

                // Second triangle
                indices.push_back(i2);
                indices.push_back(i3);
                indices.push_back(i4);
            }
        }

        return GeometryMesh<VertexType>(std::move(vertices), std::move(indices));
    }

    template<typename VertexType>
    static GeometryMesh<VertexType> CreateCone(float radius = 1.0f, float height = 2.0f, int longDiv = 24)
    {
        namespace dx = DirectX;

        // Ensure minimum divisions
        longDiv = std::max(3, longDiv);

        std::vector<VertexType> vertices;
        std::vector<unsigned short> indices;

        // Calculate constants
        const float halfHeight = height / 2.0f;
        const float angleStep = 2.0f * PI / longDiv;

        // Create base vertices around the circumference
        for (int i = 0; i < longDiv; i++)
        {
            float angle = i * angleStep;

            VertexType baseVertex;
            baseVertex.position = {
                radius * std::cos(angle),
                radius * std::sin(angle),
                -halfHeight
            };

            if constexpr (has_normal_member<VertexType>::value) {
                baseVertex.normal = { 0.0f, 0.0f, -1.0f }; // Base normal points down
            }

            vertices.push_back(baseVertex);
        }

        // Add base center point
        VertexType baseCenter;
        baseCenter.position = { 0.0f, 0.0f, -halfHeight };
        if constexpr (has_normal_member<VertexType>::value) {
            baseCenter.normal = { 0.0f, 0.0f, -1.0f }; // Base normal points down
        }

        unsigned short baseCenterIndex = static_cast<unsigned short>(vertices.size());
        vertices.push_back(baseCenter);

        // Add tip of the cone
        VertexType tip;
        tip.position = { 0.0f, 0.0f, halfHeight };
        unsigned short tipIndex = static_cast<unsigned short>(vertices.size());
        vertices.push_back(tip);

        // Create base indices (connecting each segment to the center)
        for (unsigned short i = 0; i < longDiv; i++)
        {
            indices.push_back(baseCenterIndex);
            indices.push_back((i + 1) % longDiv);
            indices.push_back(i);
        }

        // Create side face indices and calculate normals for the sides
        for (unsigned short i = 0; i < longDiv; i++)
        {
            unsigned short nextI = (i + 1) % longDiv;

            // If the vertex type supports normals, calculate the proper side normal
            if constexpr (has_normal_member<VertexType>::value) {
                // Get positions
                dx::XMVECTOR p0 = dx::XMLoadFloat3(&vertices[i].position);
                dx::XMVECTOR p1 = dx::XMLoadFloat3(&vertices[nextI].position);
                dx::XMVECTOR pTip = dx::XMLoadFloat3(&vertices[tipIndex].position);

                // Calculate two vectors along the triangle
                dx::XMVECTOR v1 = dx::XMVectorSubtract(p1, p0);
                dx::XMVECTOR v2 = dx::XMVectorSubtract(pTip, p0);

                // Calculate normal using cross product
                dx::XMVECTOR normalVec = dx::XMVector3Normalize(
                    dx::XMVector3Cross(v1, v2)
                );

                // Create new vertices for the sides with the correct normal
                VertexType sideVertex1 = vertices[i];
                VertexType sideVertex2 = vertices[nextI];
                VertexType sideVertexTip = vertices[tipIndex];

                // Store the normal
                dx::XMFLOAT3 normal;
                dx::XMStoreFloat3(&normal, normalVec);

                sideVertex1.normal = normal;
                sideVertex2.normal = normal;
                sideVertexTip.normal = normal;

                // Add these vertices to the array
                unsigned short idx1 = static_cast<unsigned short>(vertices.size());
                vertices.push_back(sideVertex1);

                unsigned short idx2 = static_cast<unsigned short>(vertices.size());
                vertices.push_back(sideVertex2);

                unsigned short idxTip = static_cast<unsigned short>(vertices.size());
                vertices.push_back(sideVertexTip);

                // Add indices for this side triangle
                indices.push_back(idx1);
                indices.push_back(idx2);
                indices.push_back(idxTip);
            }
            else {
                // If no normals, we can just use the original vertices
                indices.push_back(i);
                indices.push_back(nextI);
                indices.push_back(tipIndex);
            }
        }

        return GeometryMesh<VertexType>(std::move(vertices), std::move(indices));
    }
};