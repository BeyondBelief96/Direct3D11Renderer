#pragma once

#include <vector>
#include <type_traits>
#include <cassert>
#include <utility>
#include <string>
#include <d3d11.h>
#include <DirectXMath.h>
#include <scene.h>

#define DVTX_ELEMENT_AI_EXTRACTOR(member) static SysType Extract( const aiMesh& mesh,size_t i ) noexcept {return *reinterpret_cast<const SysType*>(&mesh.member[i]);}

/** @brief Direct3D11 vertex system namespace containing all vertex-related classes and utilities */
namespace D3
{
	/** @brief 8-bit per channel BGRA color representation.
	 *
	 *  Uses BGRA byte ordering which matches DirectX's preferred DXGI_FORMAT_R8G8B8A8_UNORM format.
	 *  Each component is an unsigned 8-bit value (0-255 range).
	 *
	 *  @note The memory layout is: [B][G][R][A] when viewed as bytes
	 */
	struct BGRAColor
	{
		/** @brief Alpha component (transparency) - 255 = fully opaque, 0 = fully transparent */
		unsigned char a;
		/** @brief Red color component */
		unsigned char r;
		/** @brief Green color component */
		unsigned char g;
		/** @brief Blue color component */
		unsigned char b;
	};

	/** @brief Defines the layout and structure of vertex data for D3D11 input layouts.
	 *
	 *  This class manages vertex element types, their memory layout, and provides
	 *  compile-time mapping to DirectX types and formats. It supports flexible
	 *  vertex configurations and automatic D3D11_INPUT_ELEMENT_DESC generation.
	 */
	class VertexLayout
	{
	public:
		/** @brief Enumeration of supported vertex element types.
		 *
		 *  Each type corresponds to a specific DirectX data format and semantic.
		 *  The order matters for vertex buffer layout calculations.
		 */
		enum ElementType
		{
			Position2D,    /**< 2D position coordinates (XMFLOAT2) */
			Position3D,    /**< 3D position coordinates (XMFLOAT3) */
			Texture2D,     /**< 2D texture coordinates (XMFLOAT2) */
			Tangent,       /**< Tangent vector for normal mapping (XMFLOAT3) */
			Bitangent,     /**< Bitangent vector for normal mapping (XMFLOAT3) */
			Normal,        /**< Surface normal vector (XMFLOAT3) */
			Float3Color,   /**< RGB color as 3 floats (XMFLOAT3) */
			Float4Color,   /**< RGBA color as 4 floats (XMFLOAT4) */
			BGRAColor,     /**< BGRA color as packed bytes (BGRAColor) */
			Count,         /**< Total number of element types - used for iteration */
		};
		/** @brief Template struct for mapping ElementType to DirectX types and formats.
		 *
		 *  This template is specialized for each ElementType to provide:
		 *  - SysType: The corresponding DirectX math type (e.g., XMFLOAT3)
		 *  - dxgiFormat: The DXGI format for the D3D11 input layout
		 *  - semantic: The HLSL semantic name for shaders
		 *  - code: Short string code for layout identification
		 *
		 *  @tparam ElementType The vertex element type to map
		 */
		template<ElementType> struct Map;
		/** @brief Specialization for 2D position coordinates */
		template<> struct Map<Position2D>
		{
			using SysType = DirectX::XMFLOAT2;  /**< DirectX 2-component float vector */
			static constexpr DXGI_FORMAT dxgiFormat = DXGI_FORMAT_R32G32_FLOAT;  /**< 32-bit float RG format */
			static constexpr const char* semantic = "Position";  /**< HLSL semantic name */
			static constexpr const char* code = "P2";  /**< Short code for layout identification */
			DVTX_ELEMENT_AI_EXTRACTOR(mVertices);
		};
		/** @brief Specialization for 3D position coordinates */
		template<> struct Map<Position3D>
		{
			using SysType = DirectX::XMFLOAT3;  /**< DirectX 3-component float vector */
			static constexpr DXGI_FORMAT dxgiFormat = DXGI_FORMAT_R32G32B32_FLOAT;  /**< 32-bit float RGB format */
			static constexpr const char* semantic = "Position";  /**< HLSL semantic name */
			static constexpr const char* code = "P3";  /**< Short code for layout identification */
			DVTX_ELEMENT_AI_EXTRACTOR(mVertices);
		};
		/** @brief Specialization for 2D texture coordinates (UV mapping) */
		template<> struct Map<Texture2D>
		{
			using SysType = DirectX::XMFLOAT2;  /**< DirectX 2-component float vector for UV coords */
			static constexpr DXGI_FORMAT dxgiFormat = DXGI_FORMAT_R32G32_FLOAT;  /**< 32-bit float RG format */
			static constexpr const char* semantic = "TexCoord";  /**< HLSL semantic name */
			static constexpr const char* code = "T2";  /**< Short code for layout identification */
			DVTX_ELEMENT_AI_EXTRACTOR(mTextureCoords[0]);
		};
		/** @brief Specialization for tangent vectors (used in normal mapping) */
		template<> struct Map<Tangent>
		{
			using SysType = DirectX::XMFLOAT3;  /**< DirectX 3-component float vector */
			static constexpr DXGI_FORMAT dxgiFormat = DXGI_FORMAT_R32G32B32_FLOAT;  /**< 32-bit float RGB format */
			static constexpr const char* semantic = "Tangent";  /**< HLSL semantic name */
			static constexpr const char* code = "Nt";  /**< Short code for layout identification */
			DVTX_ELEMENT_AI_EXTRACTOR(mTangents);
		};
		/** @brief Specialization for bitangent vectors (used in normal mapping) */
		template<> struct Map<Bitangent>
		{
			using SysType = DirectX::XMFLOAT3;  /**< DirectX 3-component float vector */
			static constexpr DXGI_FORMAT dxgiFormat = DXGI_FORMAT_R32G32B32_FLOAT;  /**< 32-bit float RGB format */
			static constexpr const char* semantic = "Bitangent";  /**< HLSL semantic name */
			static constexpr const char* code = "Nb";  /**< Short code for layout identification */
			DVTX_ELEMENT_AI_EXTRACTOR(mBitangents);
		};
		/** @brief Specialization for surface normal vectors */
		template<> struct Map<Normal>
		{
			using SysType = DirectX::XMFLOAT3;  /**< DirectX 3-component float vector */
			static constexpr DXGI_FORMAT dxgiFormat = DXGI_FORMAT_R32G32B32_FLOAT;  /**< 32-bit float RGB format */
			static constexpr const char* semantic = "Normal";  /**< HLSL semantic name */
			static constexpr const char* code = "N";  /**< Short code for layout identification */
			DVTX_ELEMENT_AI_EXTRACTOR(mNormals);
		};
		/** @brief Specialization for RGB color as 3 floating-point components */
		template<> struct Map<Float3Color>
		{
			using SysType = DirectX::XMFLOAT3;  /**< DirectX 3-component float vector for RGB */
			static constexpr DXGI_FORMAT dxgiFormat = DXGI_FORMAT_R32G32B32_FLOAT;  /**< 32-bit float RGB format */
			static constexpr const char* semantic = "Color";  /**< HLSL semantic name */
			static constexpr const char* code = "C3";  /**< Short code for layout identification */
			DVTX_ELEMENT_AI_EXTRACTOR(mColors[0]);
		};
		/** @brief Specialization for RGBA color as 4 floating-point components */
		template<> struct Map<Float4Color>
		{
			using SysType = DirectX::XMFLOAT4;  /**< DirectX 4-component float vector for RGBA */
			static constexpr DXGI_FORMAT dxgiFormat = DXGI_FORMAT_R32G32B32A32_FLOAT;  /**< 32-bit float RGBA format */
			static constexpr const char* semantic = "Color";  /**< HLSL semantic name */
			static constexpr const char* code = "C4";  /**< Short code for layout identification */
			DVTX_ELEMENT_AI_EXTRACTOR(mColors[0]);
		};
		/** @brief Specialization for BGRA color as packed 8-bit components */
		template<> struct Map<BGRAColor>
		{
			using SysType = D3::BGRAColor;  /**< Custom BGRA color struct */
			static constexpr DXGI_FORMAT dxgiFormat = DXGI_FORMAT_R8G8B8A8_UNORM;  /**< 8-bit normalized RGBA format */
			static constexpr const char* semantic = "Color";  /**< HLSL semantic name */
			static constexpr const char* code = "CB";  /**< Short code for layout identification */
			DVTX_ELEMENT_AI_EXTRACTOR(mColors[0]);
		};
		/** @brief Specialization for Count (used as sentinel/fallback) */
		template<> struct Map<Count>
		{
			using SysType = long double;  /**< Dummy type */
			static constexpr DXGI_FORMAT dxgiFormat = DXGI_FORMAT_UNKNOWN;  /**< Unknown format */
			static constexpr const char* semantic = "!INVALID!";  /**< Invalid semantic */
			static constexpr const char* code = "!INV!";  /**< Invalid code */
			DVTX_ELEMENT_AI_EXTRACTOR(mFaces);
		};

		/** @brief Bridge template to dispatch calls based on ElementType at runtime.
		 *
		 *  This template function allows calling template functions with the ElementType
		 *  determined at runtime rather than compile-time. It works by switching on the
		 *  type and invoking the templated Functor::Exec() method with the appropriate
		 *  type parameter.
		 *
		 *  @tparam Functor A template struct with a static Exec() method
		 *  @tparam Args Types of additional arguments to forward to Exec()
		 *  @param type The runtime ElementType to dispatch on
		 *  @param args Additional arguments to forward to the Exec() method
		 *  @return The return value from Functor<type>::Exec()
		 *
		 *  Example usage:
		 *  @code
		 *  template<VertexLayout::ElementType type>
		 *  struct SizeGetter {
		 *      static constexpr auto Exec() { return sizeof(VertexLayout::Map<type>::SysType); }
		 *  };
		 *  size_t size = VertexLayout::Bridge<SizeGetter>(elementType);
		 *  @endcode
		 */
		template<template<VertexLayout::ElementType> class Functor, typename... Args>
		static constexpr auto Bridge(VertexLayout::ElementType type, Args&&... args)
		{
			switch (type)
			{
			case VertexLayout::Position2D: return Functor<VertexLayout::Position2D>::Exec(std::forward<Args>(args)...);
			case VertexLayout::Position3D: return Functor<VertexLayout::Position3D>::Exec(std::forward<Args>(args)...);
			case VertexLayout::Texture2D: return Functor<VertexLayout::Texture2D>::Exec(std::forward<Args>(args)...);
			case VertexLayout::Tangent: return Functor<VertexLayout::Tangent>::Exec(std::forward<Args>(args)...);
			case VertexLayout::Bitangent: return Functor<VertexLayout::Bitangent>::Exec(std::forward<Args>(args)...);
			case VertexLayout::Normal: return Functor<VertexLayout::Normal>::Exec(std::forward<Args>(args)...);
			case VertexLayout::Float3Color: return Functor<VertexLayout::Float3Color>::Exec(std::forward<Args>(args)...);
			case VertexLayout::Float4Color: return Functor<VertexLayout::Float4Color>::Exec(std::forward<Args>(args)...);
			case VertexLayout::BGRAColor: return Functor<VertexLayout::BGRAColor>::Exec(std::forward<Args>(args)...);
			}
			assert(!"Invalid element type");
			return Functor<VertexLayout::Count>::Exec(std::forward<Args>(args)...);
		}

		/** @brief Checks if the layout contains an element of the specified type.
		 *  @tparam Type The ElementType to check for
		 *  @return True if the element type is present, false otherwise
		*/
		template<ElementType Type>
		bool Has() const noexcept
		{
			for (auto& e : elements)
			{
				if (e.GetType() == Type)
				{
					return true;
				}
			}

			return false;
		}

		/** @brief Represents a single element within a vertex layout.
		 *
		 *  Each element describes one attribute of a vertex (position, normal, etc.)
		 *  including its type, memory offset, and size. Used to build D3D11 input layouts.
		 */
		class Element
		{
		public:
			/** @brief Constructs a vertex element.
			 *  @param type The ElementType of this vertex attribute
			 *  @param offset Byte offset of this element from the start of vertex data
			 */
			Element(ElementType type, size_t offset)
				: type(type), offset(offset)
			{}
			/** @brief Gets the byte offset immediately after this element.
			 *  @return Offset + size of this element, useful for calculating next element's offset
			 */
			size_t GetOffsetAfter() const
			{
				return offset + Size();
			}
			/** @brief Gets the byte offset of this element from vertex start.
			 *  @return Byte offset within vertex data
			 */
			size_t GetOffset() const { return offset; }
			/** @brief Gets the size in bytes of this element.
			 *  @return Size in bytes of the element's data type
			 */
			size_t Size() const { return SizeOf(type); }
			/** @brief Static function to get the size of any ElementType.
			 *  @param type The ElementType to query
			 *  @return Size in bytes of the specified element type
			 *  @note Uses template Map specializations to get sizeof(SysType)
			 */
			static constexpr size_t SizeOf(ElementType type)
			{
				return Bridge<SysSizeLookup>(type);
			}
			/** @brief Gets the ElementType of this element.
			 *  @return The ElementType enum value
			 */
			ElementType GetType() const { return type; }
			/** @brief Gets the short code string for this element type.
			 *  @return Short code string (e.g., "P3", "N", "T2")
			 */
			const char* GetCode() const
			{
				return Bridge<CodeLookup>(type);
			}
			/** @brief Generates a D3D11_INPUT_ELEMENT_DESC for this element.
			 *  @return Complete D3D11 input element descriptor ready for input layout creation
			 *  @note Uses template Map specializations to get format and semantic information
			 */
			D3D11_INPUT_ELEMENT_DESC GetDesc() const
			{
				return Bridge<DescGenerate>(type, GetOffset());
			}
		private:
			/** @brief Functor for Bridge pattern to get sizeof(SysType) */
			template<ElementType type>
			struct SysSizeLookup
			{
				static constexpr auto Exec()
				{
					return sizeof(VertexLayout::Map<type>::SysType);
				}
			};
			/** @brief Functor for Bridge pattern to get element code string */
			template<ElementType type>
			struct CodeLookup
			{
				static constexpr auto Exec()
				{
					return VertexLayout::Map<type>::code;
				}
			};
			/** @brief Functor for Bridge pattern to generate D3D11_INPUT_ELEMENT_DESC */
			template<ElementType type>
			struct DescGenerate
			{
				static constexpr D3D11_INPUT_ELEMENT_DESC Exec(size_t offset)
				{
					return {
						VertexLayout::Map<type>::semantic, 0,
						VertexLayout::Map<type>::dxgiFormat,
						0, (UINT)offset, D3D11_INPUT_PER_VERTEX_DATA, 0
					};
				}
			};
		private:
			ElementType type;  /**< The type of this vertex element */
			size_t offset;     /**< Byte offset from start of vertex data */
		};
	public:
		/** @brief Finds and returns an element of the specified type.
		 *  @tparam Type The ElementType to search for
		 *  @return Reference to the first element of the specified type
		 *  @note Asserts if the element type is not found in the layout
		 */
		template<ElementType Type>
		const Element& Resolve() const
		{
			for (auto& e : elements)
			{
				if (e.GetType() == Type)
				{
					return e;
				}
			}
			assert(!"Could not resolve element type");
			return elements.front();
		}
		/** @brief Gets an element by its index in the layout.
		 *  @param i Index of the element (0-based)
		 *  @return Reference to the element at the specified index
		 *  @warning No bounds checking - ensure i < GetElementCount()
		 */
		const Element& ResolveByIndex(size_t i) const { return elements[i]; }
		/** @brief Adds a new element to the end of this layout.
		 *  @param type The ElementType to add
		 *  @return Reference to this layout for method chaining
		 *  @note Automatically calculates offset based on current layout size
		 */
		VertexLayout& Append(ElementType type)
		{
			elements.emplace_back(type, Size());
			return *this;
		}
		/** @brief Gets the total size in bytes of a vertex using this layout.
		 *  @return Total vertex size in bytes, or 0 if layout is empty
		 *  @note Calculated as the offset after the last element
		 */
		size_t Size() const { return elements.empty() ? 0u : elements.back().GetOffsetAfter(); }
		/** @brief Gets the number of elements in this layout.
		 *  @return Number of vertex attributes/elements
		 */
		size_t GetElementCount() const { return elements.size(); }
		/** @brief Generates a D3D11 input layout descriptor array.
		 *  @return Vector of D3D11_INPUT_ELEMENT_DESC ready for CreateInputLayout()
		 *  @note Each element in the layout contributes one descriptor
		 */
		std::vector<D3D11_INPUT_ELEMENT_DESC> GetD3DLayout() const
		{
			std::vector<D3D11_INPUT_ELEMENT_DESC> desc;
			desc.reserve(GetElementCount());
			for (const auto& e : elements)
			{
				desc.push_back(e.GetDesc());
			}
			return desc;
		}
		/** @brief Generates a compact string code representing this layout.
		 *  @return String with abbreviated codes for each element (e.g., "P3NT2" for Position3D+Normal+Texture2D)
		 *  @note Useful for layout identification, caching, and debugging
		 *
		 *  Element codes:
		 *  - P2: Position2D, P3: Position3D
		 *  - T2: Texture2D, N: Normal
		 *  - Nt: Tangent, Nb: Bitangent
		 *  - C3: Float3Color, C4: Float4Color, CB: BGRAColor
		 */
		std::string GetCode() const
		{
			std::string code;
			for (const auto& e : elements)
			{
				code += e.GetCode();
			}
			return code;
		}
	private:
		std::vector<Element> elements;  /**< Ordered list of vertex elements */
	};

	/** @brief Represents a single vertex with dynamic attribute access.
	 *
	 *  Provides type-safe access to vertex attributes based on a VertexLayout.
	 *  Acts as a view into raw vertex data, allowing both reading and writing
	 *  of vertex components. The vertex data is not owned by this class.
	 *
	 *  @note This is a lightweight wrapper around a char* pointer and layout reference
	 */
	class Vertex
	{
		friend class VertexBuffer;
	private:
		/** @brief Functor for Bridge pattern to set attributes with type checking */
		template<VertexLayout::ElementType type>
		struct AttributeSetting
		{
			template<typename T>
			static constexpr auto Exec(Vertex* pVertex, char* pAttribute, T&& val)
			{
				return pVertex->SetAttribute<type>(pAttribute, std::forward<T>(val));
			}
		};
	public:
		/** @brief Gets a reference to a vertex attribute of the specified type.
		 *  @tparam Type The ElementType of the attribute to access
		 *  @return Mutable reference to the attribute data cast to appropriate type
		 *  @note Uses template Map specialization to determine the return type
		 *  @warning Asserts if the element type is not present in the layout
		 *
		 *  Example usage:
		 *  @code
		 *  auto& pos = vertex.Attr<VertexLayout::Position3D>();
		 *  pos.x = 1.0f; // Modify position
		 *  @endcode
		 */
		template<VertexLayout::ElementType Type>
		auto& Attr()
		{
			auto pAttribute = pData + layout.Resolve<Type>().GetOffset();
			return *reinterpret_cast<typename VertexLayout::Map<Type>::SysType*>(pAttribute);
		}
		/** @brief Sets a vertex attribute by its index position in the layout.
		 *  @tparam T The type of the value being assigned (automatically deduced)
		 *  @param i Index of the attribute in the layout (0-based)
		 *  @param val The value to assign (forwarded to preserve value category)
		 *  @note Uses SFINAE to ensure type compatibility at compile time
		 *  @warning Asserts if the value type is not assignable to the target type
		 */
		template<typename T>
		void SetAttributeByIndex(size_t i, T&& val)
		{
			const auto& element = layout.ResolveByIndex(i);
			auto pAttribute = pData + element.GetOffset();
			VertexLayout::Bridge<AttributeSetting>(
				element.GetType(), this, pAttribute, std::forward<T>(val)
			);
		}
		/** @brief Variadic template helper to set multiple attributes by index.
		 *  @tparam First Type of the first attribute value
		 *  @tparam Rest Types of remaining attribute values
		 *  @param i Starting index for attribute assignment
		 *  @param first First attribute value to assign
		 *  @param rest Remaining attribute values to assign sequentially
		 *  @note Recursively calls SetAttributeByIndex for each parameter
		 */
		template<typename First, typename ...Rest>
		void SetAttributeByIndex(size_t i, First&& first, Rest&&... rest)
		{
			SetAttributeByIndex(i, std::forward<First>(first));
			SetAttributeByIndex(i + 1, std::forward<Rest>(rest)...);
		}
	public:
		/** @brief Constructor - creates a Vertex instance.
		 *  @param pData Pointer to the raw vertex data (must not be null)
		 *  @param layout Reference to the vertex layout describing the data structure
		 *  @note Asserts if pData is null to catch programming errors early
		 */
		Vertex(char* pData, const VertexLayout& layout)
			: pData(pData), layout(layout)
		{
			assert(pData != nullptr);
		}
	private:
		/** @brief Template helper to set an attribute with compile-time type checking.
		 *  @tparam DestLayoutType The target ElementType for the attribute
		 *  @tparam SrcType The source type of the value being assigned
		 *  @param pAttribute Pointer to the attribute location in vertex data
		 *  @param val The value to assign (forwarded to preserve value category)
		 *  @note Uses SFINAE with std::is_assignable to ensure type compatibility
		 *  @warning Asserts at runtime if types are incompatible (should never happen with proper usage)
		 */
		template<VertexLayout::ElementType DestLayoutType, typename SrcType>
		void SetAttribute(char* pAttribute, SrcType&& val)
		{
			using Dest = typename VertexLayout::Map<DestLayoutType>::SysType;
			if constexpr (std::is_assignable<Dest, SrcType>::value)
			{
				*reinterpret_cast<Dest*>(pAttribute) = val;
			}
			else
			{
				assert(!"Parameter attribute type mismatch");
			}
		}
	private:
		char* pData = nullptr;        /**< Pointer to the raw vertex data */
		const VertexLayout& layout;   /**< Reference to the vertex layout */
	};

	/** @brief Read-only wrapper for Vertex providing const access to attributes.
	 *
	 *  Provides the same attribute access interface as Vertex but prevents modification.
	 *  Useful when you want to pass vertex data around without allowing changes.
	 *
	 *  @note This class stores a copy of the Vertex, not a reference
	 */
	class ConstVertex
	{
	public:
		/** @brief Constructs a const vertex from a mutable vertex.
		 *  @param v The vertex to create a const view of
		 *  @note Creates a copy of the vertex for const access
		 */
		ConstVertex(const Vertex& v)
			: vertex(v)
		{}
		/** @brief Gets a const reference to a vertex attribute.
		 *  @tparam Type The ElementType of the attribute to access
		 *  @return Const reference to the attribute data
		 *  @note Implementation uses const_cast internally but maintains const interface
		 */
		template<VertexLayout::ElementType Type>
		const auto& Attr() const
		{
			return const_cast<Vertex&>(vertex).Attr<Type>();
		}
	private:
		Vertex vertex;  /**< Copy of the vertex data for const access */
	};

	/** @brief Dynamic vertex buffer with flexible layout support.
	 *
	 *  Manages a collection of vertices in a contiguous memory buffer.
	 *  Supports adding vertices, accessing individual vertices, and provides
	 *  methods for integration with D3D11 vertex buffers.
	 *
	 *  The buffer owns its data and manages memory automatically as vertices are added.
	 *
	 *  @note All vertices in a buffer share the same layout
	 */
	class VertexBuffer
	{
	public:

		template<VertexLayout::ElementType type>
		struct AttributeAiMeshFill
		{
			static constexpr void Exec(VertexBuffer* pBuffer, const aiMesh& mesh) noexcept
			{
				for (auto end = mesh.mNumVertices, i = 0u; i < end; i++)
				{
					(*pBuffer)[i].Attr<type>() = VertexLayout::Map<type>::Extract(mesh, i);
				}
			}
		};

		/** @brief Constructs a vertex buffer with the specified layout.
		 *  @param layout The vertex layout describing the structure of each vertex
		 *  @note The layout is moved into the buffer for efficiency
		 */
		VertexBuffer(VertexLayout layout, size_t size = 0u)
			: layout(std::move(layout))
		{
			Resize(size);
		}

		VertexBuffer(VertexLayout layout_in, const aiMesh& mesh) : layout(std::move(layout_in))
		{
			Resize(mesh.mNumVertices);
			for (size_t i = 0, end = layout.GetElementCount(); i < end; i++)
			{
				VertexLayout::Bridge<AttributeAiMeshFill>(layout.ResolveByIndex(i).GetType(), this, mesh);
			}
		}
		/** @brief Gets a pointer to the raw vertex buffer data.
		 *  @return Const pointer to the buffer data, suitable for D3D11 vertex buffer creation
		 */
		const char* GetData() const { return buffer.data(); }
		/** @brief Gets the vertex layout used by this buffer.
		 *  @return Const reference to the vertex layout
		 */

		const VertexLayout& GetLayout() const { return layout; }
		/** @brief Gets the number of vertices in the buffer.
		 *  @return Number of complete vertices stored in the buffer
		 */
		size_t Size() const { return buffer.size() / layout.Size(); }

		/** @brief Resizes the buffer to the specified size.*/
		void Resize(size_t newSize) noexcept
		{
			const auto size = Size();
			if (size < newSize)
			{
				buffer.resize(buffer.size() + layout.Size() * (newSize - size));
			}
		}

		/** @brief Gets the total size of the buffer in bytes.
		*  @return Total buffer size in bytes
		*/
		size_t SizeBytes() const { return buffer.size(); }
		/** @brief Adds a new vertex to the buffer using perfect forwarding.
		 *  @tparam Params Types of the vertex attribute values (automatically deduced)
		 *  @param params Vertex attribute values in layout order
		 *  @note Parameter count must exactly match the number of elements in the layout
		 *  @warning Asserts if parameter count doesn't match layout element count
		 *
		 *  Example usage:
		 *  @code
		 *  // For layout with Position3D + Normal + Texture2D:
		 *  buffer.EmplaceBack(
		 *      DirectX::XMFLOAT3{1.0f, 2.0f, 3.0f},  // position
		 *      DirectX::XMFLOAT3{0.0f, 1.0f, 0.0f},  // normal
		 *      DirectX::XMFLOAT2{0.5f, 0.5f}         // texture coords
		 *  );
		 *  @endcode
		 */
		template<typename ...Params>
		void EmplaceBack(Params&&... params)
		{
			assert(sizeof...(params) == layout.GetElementCount() && "Param count doesn't match number of vertex elements");
			buffer.resize(buffer.size() + layout.Size());
			Back().SetAttributeByIndex(0u, std::forward<Params>(params)...);
		}
		/** @brief Gets a mutable reference to the last vertex in the buffer.
		 *  @return Vertex wrapper for the last vertex
		 *  @warning Asserts if the buffer is empty
		 */
		Vertex Back()
		{
			assert(buffer.size() != 0u);
			return Vertex(buffer.data() + buffer.size() - layout.Size(), layout);
		}
		/** @brief Gets a mutable reference to the first vertex in the buffer.
		 *  @return Vertex wrapper for the first vertex
		 *  @warning Asserts if the buffer is empty
		 */
		Vertex Front()
		{
			assert(buffer.size() != 0u);
			return Vertex(buffer.data(), layout);
		}
		/** @brief Gets a mutable reference to the vertex at the specified index.
		 *  @param i Index of the vertex (0-based)
		 *  @return Vertex wrapper for the vertex at index i
		 *  @warning Asserts if index is out of bounds
		 */
		Vertex operator[](size_t i)
		{
			assert(i < Size());
			return Vertex(buffer.data() + layout.Size() * i, layout);
		}
		/** @brief Gets a const reference to the last vertex in the buffer.
		 *  @return ConstVertex wrapper for the last vertex
		 *  @warning Asserts if the buffer is empty
		 */
		ConstVertex Back() const { return const_cast<VertexBuffer*>(this)->Back(); }
		/** @brief Gets a const reference to the first vertex in the buffer.
		 *  @return ConstVertex wrapper for the first vertex
		 *  @warning Asserts if the buffer is empty
		 */
		ConstVertex Front() const { return const_cast<VertexBuffer*>(this)->Front(); }
		/** @brief Gets a const reference to the vertex at the specified index.
		 *  @param i Index of the vertex (0-based)
		 *  @return ConstVertex wrapper for the vertex at index i
		 *  @warning Asserts if index is out of bounds
		 */
		ConstVertex operator[](size_t i) const { return const_cast<VertexBuffer&>(*this)[i]; }
	private:
		std::vector<char> buffer;  /**< Raw vertex data storage */
		VertexLayout layout;       /**< Layout describing vertex structure */
	};
}
