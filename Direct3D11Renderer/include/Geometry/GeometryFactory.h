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
    static GeometryMesh<VertexType> CreatePyramid(float radius = 1.0f, float height = 2.0f, int sides = 4)
    {
        namespace dx = DirectX;

        // Ensure minimum sides
        sides = std::max(3, sides);

        std::vector<VertexType> vertices;
        std::vector<unsigned short> indices;

        // Calculate constants
        const float halfHeight = height / 2.0f;
        const float angleStep = 2.0f * PI / sides;

        // First, create the base vertices
        for (int i = 0; i < sides; i++)
        {
            float angle = i * angleStep;

            // Create base vertex
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

        // Add apex of pyramid
        VertexType apex;
        apex.position = { 0.0f, 0.0f, halfHeight };
        unsigned short apexIndex = static_cast<unsigned short>(vertices.size());
        vertices.push_back(apex);

        // Now create the base triangles (clockwise winding when viewed from -Z)
        for (int i = 0; i < sides; i++)
        {
            indices.push_back(baseCenterIndex); // center
            indices.push_back(i);               // current
            indices.push_back((i + 1) % sides); // next
        }

        // Now create side faces
        for (int i = 0; i < sides; i++)
        {
            int nextI = (i + 1) % sides;

            // Get the base vertex positions
            dx::XMFLOAT3 currentPos = vertices[i].position;
            dx::XMFLOAT3 nextPos = vertices[nextI].position;
            dx::XMFLOAT3 apexPos = vertices[apexIndex].position;

            // Calculate two vectors along the face
            dx::XMVECTOR v1 = dx::XMVectorSubtract(
                dx::XMLoadFloat3(&nextPos),
                dx::XMLoadFloat3(&apexPos)
            );

            dx::XMVECTOR v2 = dx::XMVectorSubtract(
                dx::XMLoadFloat3(&currentPos),
                dx::XMLoadFloat3(&apexPos)
            );

            // Calculate the face normal using cross product
            // Using vector order to ensure normal points outward (for clockwise winding)
            dx::XMVECTOR faceNormalVec = dx::XMVector3Normalize(
                dx::XMVector3Cross(v1, v2)
            );

            // Store the normal
            dx::XMFLOAT3 faceNormal;
            dx::XMStoreFloat3(&faceNormal, faceNormalVec);

            // Create copies of vertices for the side face with the correct normal
            VertexType currentVertexSide = vertices[i]; // Copy base vertex
            VertexType nextVertexSide = vertices[nextI]; // Copy next base vertex
            VertexType apexVertexSide = vertices[apexIndex]; // Copy apex

            if constexpr (has_normal_member<VertexType>::value) {
                currentVertexSide.normal = faceNormal;
                nextVertexSide.normal = faceNormal;
                apexVertexSide.normal = faceNormal;
            }

            // Add these vertices to the array
            unsigned short currentSideIndex = static_cast<unsigned short>(vertices.size());
            vertices.push_back(currentVertexSide);

            unsigned short nextSideIndex = static_cast<unsigned short>(vertices.size());
            vertices.push_back(nextVertexSide);

            unsigned short apexSideIndex = static_cast<unsigned short>(vertices.size());
            vertices.push_back(apexVertexSide);

            // Add indices for the side triangle (clockwise winding when viewed from outside)
            indices.push_back(currentSideIndex);
            indices.push_back(apexSideIndex);
            indices.push_back(nextSideIndex);
        }

        return GeometryMesh<VertexType>(std::move(vertices), std::move(indices));
    }

    template<typename VertexType>
    static GeometryMesh<VertexType> CreatePlane(float width = 2.0f, float height = 2.0f, int divisionsX = 1, int divisionsY = 1)
    {
        namespace dx = DirectX;

        // Ensure minimum divisions
        divisionsX = std::max(1, divisionsX);
        divisionsY = std::max(1, divisionsY);

        // Calculate the number of vertices in each dimension
        const int verticesX = divisionsX + 1;
        const int verticesY = divisionsY + 1;

        // Create vertex array
        std::vector<VertexType> vertices(verticesX * verticesY);

        // Calculate dimensions and division sizes
        const float halfWidth = width / 2.0f;
        const float halfHeight = height / 2.0f;
        const float divisionSizeX = width / static_cast<float>(divisionsX);
        const float divisionSizeY = height / static_cast<float>(divisionsY);

        // Generate vertices starting from bottom-left
        const auto bottomLeft = dx::XMVectorSet(-halfWidth, -halfHeight, 0.0f, 0.0f);

        int vertexIndex = 0;
        for (int y = 0; y < verticesY; y++)
        {
            const float yPos = static_cast<float>(y) * divisionSizeY - halfHeight;

            for (int x = 0; x < verticesX; x++, vertexIndex++)
            {
                const float xPos = static_cast<float>(x) * divisionSizeX - halfWidth;

                // Set position
                vertices[vertexIndex].position = { xPos, yPos, 0.0f };

                // Set texture coordinates (important for our texture mapping)
                if constexpr (has_texcoord_member<VertexType>::value) {
                    float u = static_cast<float>(x) / divisionsX;
                    float v = static_cast<float>(y) / divisionsY;
                    vertices[vertexIndex].texCoord = { u, v };
                }
            }
        }

        // Generate indices
        std::vector<unsigned short> indices;
        indices.reserve(divisionsX * divisionsY * 6); // 2 triangles per quad, 3 vertices per triangle

        // Helper lambda to convert x,y coordinates to vertex index
        const auto vxy2i = [verticesX](size_t x, size_t y) -> unsigned short {
            return static_cast<unsigned short>(y * verticesX + x);
            };

        // Generate quads
        for (size_t y = 0; y < divisionsY; y++)
        {
            for (size_t x = 0; x < divisionsX; x++)
            {
                // Get the four corner indices of this quad
                const std::array<unsigned short, 4> indexArray = {
                    vxy2i(x, y),         // Bottom-left
                    vxy2i(x + 1, y),     // Bottom-right
                    vxy2i(x, y + 1),     // Top-left
                    vxy2i(x + 1, y + 1)  // Top-right
                };

                // First triangle (bottom-left, top-left, bottom-right)
                indices.push_back(indexArray[0]); // Bottom-left
                indices.push_back(indexArray[2]); // Top-left
                indices.push_back(indexArray[1]); // Bottom-right

                // Second triangle (bottom-right, top-left, top-right)
                indices.push_back(indexArray[1]); // Bottom-right
                indices.push_back(indexArray[2]); // Top-left
                indices.push_back(indexArray[3]); // Top-right
            }
        }

        return GeometryMesh<VertexType>(std::move(vertices), std::move(indices));
    }
};