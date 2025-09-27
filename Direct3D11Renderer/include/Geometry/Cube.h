#pragma once

#include "Vertex.h"
#include "IndexedTriangleList.h"
#include <DirectXMath.h>

class Cube
{
public:
	static IndexedTriangleList Make()
	{
		using D3::VertexLayout;
		namespace dx = DirectX;

		VertexLayout vl;
		vl.Append(VertexLayout::Position3D);
		vl.Append(VertexLayout::Normal);
		vl.Append(VertexLayout::Texture2D);

		return MakeIndependentTextured(std::move(vl));
	}

	static IndexedTriangleList MakeSolid()
	{
		using D3::VertexLayout;
		namespace dx = DirectX;

		VertexLayout vl;
		vl.Append(VertexLayout::Position3D);
		vl.Append(VertexLayout::Normal);

		return MakeIndependentSolid(std::move(vl));
	}

	static IndexedTriangleList MakeIndependentTextured(D3::VertexLayout layout)
	{
		namespace dx = DirectX;
		constexpr float side = 1.0f / 2.0f;

		D3::VertexBuffer vb{ std::move(layout) };

		// Create 24 vertices (4 for each of the 6 faces) for independent normals
		// Near face (negative Z)
		vb.EmplaceBack(dx::XMFLOAT3{ -side, -side, -side }, dx::XMFLOAT3{ 0.0f, 0.0f, -1.0f }, dx::XMFLOAT2{ 0.0f, 1.0f });
		vb.EmplaceBack(dx::XMFLOAT3{ side, -side, -side }, dx::XMFLOAT3{ 0.0f, 0.0f, -1.0f }, dx::XMFLOAT2{ 1.0f, 1.0f });
		vb.EmplaceBack(dx::XMFLOAT3{ -side, side, -side }, dx::XMFLOAT3{ 0.0f, 0.0f, -1.0f }, dx::XMFLOAT2{ 0.0f, 0.0f });
		vb.EmplaceBack(dx::XMFLOAT3{ side, side, -side }, dx::XMFLOAT3{ 0.0f, 0.0f, -1.0f }, dx::XMFLOAT2{ 1.0f, 0.0f });

		// Far face (positive Z)
		vb.EmplaceBack(dx::XMFLOAT3{ -side, -side, side }, dx::XMFLOAT3{ 0.0f, 0.0f, 1.0f }, dx::XMFLOAT2{ 1.0f, 1.0f });
		vb.EmplaceBack(dx::XMFLOAT3{ side, -side, side }, dx::XMFLOAT3{ 0.0f, 0.0f, 1.0f }, dx::XMFLOAT2{ 0.0f, 1.0f });
		vb.EmplaceBack(dx::XMFLOAT3{ -side, side, side }, dx::XMFLOAT3{ 0.0f, 0.0f, 1.0f }, dx::XMFLOAT2{ 1.0f, 0.0f });
		vb.EmplaceBack(dx::XMFLOAT3{ side, side, side }, dx::XMFLOAT3{ 0.0f, 0.0f, 1.0f }, dx::XMFLOAT2{ 0.0f, 0.0f });

		// Left face (negative X)
		vb.EmplaceBack(dx::XMFLOAT3{ -side, -side, -side }, dx::XMFLOAT3{ -1.0f, 0.0f, 0.0f }, dx::XMFLOAT2{ 1.0f, 1.0f });
		vb.EmplaceBack(dx::XMFLOAT3{ -side, side, -side }, dx::XMFLOAT3{ -1.0f, 0.0f, 0.0f }, dx::XMFLOAT2{ 1.0f, 0.0f });
		vb.EmplaceBack(dx::XMFLOAT3{ -side, -side, side }, dx::XMFLOAT3{ -1.0f, 0.0f, 0.0f }, dx::XMFLOAT2{ 0.0f, 1.0f });
		vb.EmplaceBack(dx::XMFLOAT3{ -side, side, side }, dx::XMFLOAT3{ -1.0f, 0.0f, 0.0f }, dx::XMFLOAT2{ 0.0f, 0.0f });

		// Right face (positive X)
		vb.EmplaceBack(dx::XMFLOAT3{ side, -side, -side }, dx::XMFLOAT3{ 1.0f, 0.0f, 0.0f }, dx::XMFLOAT2{ 0.0f, 1.0f });
		vb.EmplaceBack(dx::XMFLOAT3{ side, side, -side }, dx::XMFLOAT3{ 1.0f, 0.0f, 0.0f }, dx::XMFLOAT2{ 0.0f, 0.0f });
		vb.EmplaceBack(dx::XMFLOAT3{ side, -side, side }, dx::XMFLOAT3{ 1.0f, 0.0f, 0.0f }, dx::XMFLOAT2{ 1.0f, 1.0f });
		vb.EmplaceBack(dx::XMFLOAT3{ side, side, side }, dx::XMFLOAT3{ 1.0f, 0.0f, 0.0f }, dx::XMFLOAT2{ 1.0f, 0.0f });

		// Bottom face (negative Y)
		vb.EmplaceBack(dx::XMFLOAT3{ -side, -side, -side }, dx::XMFLOAT3{ 0.0f, -1.0f, 0.0f }, dx::XMFLOAT2{ 0.0f, 0.0f });
		vb.EmplaceBack(dx::XMFLOAT3{ side, -side, -side }, dx::XMFLOAT3{ 0.0f, -1.0f, 0.0f }, dx::XMFLOAT2{ 1.0f, 0.0f });
		vb.EmplaceBack(dx::XMFLOAT3{ -side, -side, side }, dx::XMFLOAT3{ 0.0f, -1.0f, 0.0f }, dx::XMFLOAT2{ 0.0f, 1.0f });
		vb.EmplaceBack(dx::XMFLOAT3{ side, -side, side }, dx::XMFLOAT3{ 0.0f, -1.0f, 0.0f }, dx::XMFLOAT2{ 1.0f, 1.0f });

		// Top face (positive Y)
		vb.EmplaceBack(dx::XMFLOAT3{ -side, side, -side }, dx::XMFLOAT3{ 0.0f, 1.0f, 0.0f }, dx::XMFLOAT2{ 0.0f, 1.0f });
		vb.EmplaceBack(dx::XMFLOAT3{ side, side, -side }, dx::XMFLOAT3{ 0.0f, 1.0f, 0.0f }, dx::XMFLOAT2{ 1.0f, 1.0f });
		vb.EmplaceBack(dx::XMFLOAT3{ -side, side, side }, dx::XMFLOAT3{ 0.0f, 1.0f, 0.0f }, dx::XMFLOAT2{ 0.0f, 0.0f });
		vb.EmplaceBack(dx::XMFLOAT3{ side, side, side }, dx::XMFLOAT3{ 0.0f, 1.0f, 0.0f }, dx::XMFLOAT2{ 1.0f, 0.0f });

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

		return { std::move(vb), std::move(indices) };
	}

	static IndexedTriangleList MakeIndependentSolid(D3::VertexLayout layout)
	{
		namespace dx = DirectX;
		constexpr float side = 1.0f / 2.0f;

		D3::VertexBuffer vb{ std::move(layout) };

		// Create 24 vertices (4 for each of the 6 faces) for independent normals
		// Near face (negative Z)
		vb.EmplaceBack(dx::XMFLOAT3{ -side, -side, -side }, dx::XMFLOAT3{ 0.0f, 0.0f, -1.0f });
		vb.EmplaceBack(dx::XMFLOAT3{ side, -side, -side }, dx::XMFLOAT3{ 0.0f, 0.0f, -1.0f });
		vb.EmplaceBack(dx::XMFLOAT3{ -side, side, -side }, dx::XMFLOAT3{ 0.0f, 0.0f, -1.0f });
		vb.EmplaceBack(dx::XMFLOAT3{ side, side, -side }, dx::XMFLOAT3{ 0.0f, 0.0f, -1.0f });

		// Far face (positive Z)
		vb.EmplaceBack(dx::XMFLOAT3{ -side, -side, side }, dx::XMFLOAT3{ 0.0f, 0.0f, 1.0f });
		vb.EmplaceBack(dx::XMFLOAT3{ side, -side, side }, dx::XMFLOAT3{ 0.0f, 0.0f, 1.0f });
		vb.EmplaceBack(dx::XMFLOAT3{ -side, side, side }, dx::XMFLOAT3{ 0.0f, 0.0f, 1.0f });
		vb.EmplaceBack(dx::XMFLOAT3{ side, side, side }, dx::XMFLOAT3{ 0.0f, 0.0f, 1.0f });

		// Left face (negative X)
		vb.EmplaceBack(dx::XMFLOAT3{ -side, -side, -side }, dx::XMFLOAT3{ -1.0f, 0.0f, 0.0f });
		vb.EmplaceBack(dx::XMFLOAT3{ -side, side, -side }, dx::XMFLOAT3{ -1.0f, 0.0f, 0.0f });
		vb.EmplaceBack(dx::XMFLOAT3{ -side, -side, side }, dx::XMFLOAT3{ -1.0f, 0.0f, 0.0f });
		vb.EmplaceBack(dx::XMFLOAT3{ -side, side, side }, dx::XMFLOAT3{ -1.0f, 0.0f, 0.0f });

		// Right face (positive X)
		vb.EmplaceBack(dx::XMFLOAT3{ side, -side, -side }, dx::XMFLOAT3{ 1.0f, 0.0f, 0.0f });
		vb.EmplaceBack(dx::XMFLOAT3{ side, side, -side }, dx::XMFLOAT3{ 1.0f, 0.0f, 0.0f });
		vb.EmplaceBack(dx::XMFLOAT3{ side, -side, side }, dx::XMFLOAT3{ 1.0f, 0.0f, 0.0f });
		vb.EmplaceBack(dx::XMFLOAT3{ side, side, side }, dx::XMFLOAT3{ 1.0f, 0.0f, 0.0f });

		// Bottom face (negative Y)
		vb.EmplaceBack(dx::XMFLOAT3{ -side, -side, -side }, dx::XMFLOAT3{ 0.0f, -1.0f, 0.0f });
		vb.EmplaceBack(dx::XMFLOAT3{ side, -side, -side }, dx::XMFLOAT3{ 0.0f, -1.0f, 0.0f });
		vb.EmplaceBack(dx::XMFLOAT3{ -side, -side, side }, dx::XMFLOAT3{ 0.0f, -1.0f, 0.0f });
		vb.EmplaceBack(dx::XMFLOAT3{ side, -side, side }, dx::XMFLOAT3{ 0.0f, -1.0f, 0.0f });

		// Top face (positive Y)
		vb.EmplaceBack(dx::XMFLOAT3{ -side, side, -side }, dx::XMFLOAT3{ 0.0f, 1.0f, 0.0f });
		vb.EmplaceBack(dx::XMFLOAT3{ side, side, -side }, dx::XMFLOAT3{ 0.0f, 1.0f, 0.0f });
		vb.EmplaceBack(dx::XMFLOAT3{ -side, side, side }, dx::XMFLOAT3{ 0.0f, 1.0f, 0.0f });
		vb.EmplaceBack(dx::XMFLOAT3{ side, side, side }, dx::XMFLOAT3{ 0.0f, 1.0f, 0.0f });

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

		return { std::move(vb), std::move(indices) };
	}
};