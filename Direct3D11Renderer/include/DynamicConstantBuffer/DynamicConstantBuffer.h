#pragma once

#include <DirectXMath.h>
#include <vector>
#include <memory>
#include <string>
#include <unordered_map>
#include <optional>
#include <cassert>

namespace D3
{
    namespace dx = DirectX;

    /// <summary>
    /// Enumeration of supported HLSL data types for dynamic constant buffers.
    /// Each type corresponds to a specific HLSL shader type with proper GPU alignment.
    /// These types follow DirectX 11 constant buffer packing rules where elements
    /// must be aligned to their natural boundary and cannot cross 16-byte boundaries.
    /// </summary>
    enum class ElementType
    {
        Float,      ///< HLSL float - 4 bytes, 4-byte aligned
        Float2,     ///< HLSL float2 - 8 bytes, 8-byte aligned
        Float3,     ///< HLSL float3 - 12 bytes, 16-byte aligned (padded to 16)
        Float4,     ///< HLSL float4 - 16 bytes, 16-byte aligned
        Matrix4x4,  ///< HLSL float4x4 - 64 bytes, 16-byte aligned (4x float4 rows)
        Bool,       ///< HLSL bool - 4 bytes, 4-byte aligned (HLSL bools are always 32-bit)
        Struct,     ///< Container type for grouping multiple elements with proper alignment
        Array,      ///< Container type for repeated elements with consistent stride
        Empty       ///< Invalid/uninitialized element type - used for error handling
    };

    /// <summary>
    /// Contains size, alignment, and signature information for each ElementType.
    /// Used by the TypeRegistry to provide consistent type metadata across the system.
    /// </summary>
    struct TypeInfo
    {
        size_t size;        ///< Size in bytes of this type
        size_t alignment;   ///< Required byte alignment for this type
        std::string signature; ///< Unique string identifier for layout caching
    };

    /// <summary>
    /// Singleton registry that maps C++ types to ElementType enum values and provides
    /// type metadata for layout calculations. This class ensures consistent type
    /// information across the entire dynamic constant buffer system.
    /// </summary>
    class TypeRegistry
    {
    public:
        /// <summary>
        /// Retrieves type information (size, alignment, signature) for a given ElementType.
        /// </summary>
        /// <param name="type">The ElementType to get information for</param>
        /// <returns>Reference to TypeInfo containing size, alignment, and signature</returns>
        static const TypeInfo& GetTypeInfo(ElementType type);

        /// <summary>
        /// Checks if the given ElementType represents a concrete system type (not Struct/Array/Empty).
        /// </summary>
        /// <param name="type">The ElementType to validate</param>
        /// <returns>True if type is a concrete system type (Float, Float2, etc.)</returns>
        static bool IsValidSystemType(ElementType type);

        /// <summary>
        /// Template function that maps C++ types to their corresponding ElementType.
        /// Specialized for: float->Float, DirectX::XMFLOAT2->Float2, etc.
        /// </summary>
        /// <typeparam name="T">C++ type to map</typeparam>
        /// <returns>Corresponding ElementType enum value</returns>
        template<typename T>
        static ElementType GetElementType();

    private:
        /// <summary>
        /// Static lookup table mapping ElementType values to their TypeInfo.
        /// Initialized with proper HLSL size and alignment requirements.
        /// </summary>
        static const std::unordered_map<ElementType, TypeInfo> typeMap;
    };

    /// <summary>
    /// Core building block of the dynamic constant buffer layout system.
    /// Represents a single element in a constant buffer layout, which can be a primitive type,
    /// struct, or array. Handles HLSL alignment rules and offset calculations automatically.
    /// Forms a tree structure where structs and arrays can contain child elements.
    /// </summary>
    class LayoutElement
    {
    public:
        /// <summary>Default constructor creates an Empty element</summary>
        LayoutElement() = default;

        /// <summary>Creates a LayoutElement of the specified type</summary>
        /// <param name="type">The ElementType for this element</param>
        explicit LayoutElement(ElementType type);

        /// <summary>Copy constructor - properly handles deep copying of nested structures</summary>
        LayoutElement(const LayoutElement& other);

        /// <summary>Copy assignment operator - properly handles deep copying of nested structures</summary>
        LayoutElement& operator=(const LayoutElement& other);

        /// <summary>Move constructor - efficient transfer of ownership</summary>
        LayoutElement(LayoutElement&& other) noexcept = default;

        /// <summary>Move assignment operator - efficient transfer of ownership</summary>
        LayoutElement& operator=(LayoutElement&& other) noexcept = default;

        // Element information
        /// <summary>Checks if this element has been initialized with a valid type</summary>
        /// <returns>True if type is not Empty</returns>
        bool Exists() const noexcept { return type != ElementType::Empty; }

        /// <summary>Gets the ElementType of this layout element</summary>
        /// <returns>The ElementType enum value</returns>
        ElementType GetType() const noexcept { return type; }

        /// <summary>Gets the byte offset of this element within its parent structure</summary>
        /// <returns>Byte offset from parent's start, or throws if not finalized</returns>
        size_t GetOffset() const;

        /// <summary>Gets the total size in bytes of this element (including padding)</summary>
        /// <returns>Size in bytes</returns>
        size_t GetSize() const;

        /// <summary>Generates a unique string signature for this element's layout</summary>
        /// <returns>String signature used for layout caching</returns>
        std::string GetSignature() const;

        // Struct operations
        /// <summary>Adds a new member to this struct element</summary>
        /// <param name="memberType">Type of the member to add</param>
        /// <param name="name">Name of the member (must be valid C++ identifier)</param>
        /// <returns>Reference to the newly added member element</returns>
        LayoutElement& AddMember(ElementType memberType, const std::string& name);

        /// <summary>Accesses a struct member by name (non-const version)</summary>
        /// <param name="name">Name of the member to access</param>
        /// <returns>Reference to the member element</returns>
        LayoutElement& operator[](const std::string& name);

        /// <summary>Accesses a struct member by name (const version)</summary>
        /// <param name="name">Name of the member to access</param>
        /// <returns>Const reference to the member element</returns>
        const LayoutElement& operator[](const std::string& name) const;

        // Array operations
        /// <summary>Configures this element as an array of the specified type and count</summary>
        /// <param name="elementType">Type of elements in the array</param>
        /// <param name="count">Number of elements in the array</param>
        /// <returns>Reference to this element for method chaining</returns>
        LayoutElement& SetArrayType(ElementType elementType, size_t count);

        /// <summary>Gets the element type of array elements (non-const version)</summary>
        /// <returns>Reference to the array's element type</returns>
        LayoutElement& GetArrayElementType();

        /// <summary>Gets the element type of array elements (const version)</summary>
        /// <returns>Const reference to the array's element type</returns>
        const LayoutElement& GetArrayElementType() const;

        /// <summary>Gets the number of elements in this array</summary>
        /// <returns>Array size, or 0 if not an array</returns>
        size_t GetArraySize() const;

        /// <summary>Calculates the offset and element type for a specific array index</summary>
        /// <param name="offset">Base offset of the array</param>
        /// <param name="index">Array index to calculate offset for</param>
        /// <returns>Pair of (calculated offset, element type pointer)</returns>
        std::pair<size_t, const LayoutElement*> CalculateArrayOffset(size_t offset, size_t index) const;

        // Layout finalization
        /// <summary>
        /// Calculates final offsets for this element and all children, applying HLSL alignment rules.
        /// Must be called before the layout can be used for data access.
        /// </summary>
        /// <param name="startOffset">Starting byte offset for this element</param>
        /// <returns>Next available offset after this element (including padding)</returns>
        size_t Finalize(size_t startOffset);

        /// <summary>
        /// Type-safe offset resolution with compile-time type checking.
        /// Verifies that the requested C++ type matches this element's type.
        /// </summary>
        /// <typeparam name="T">C++ type to resolve offset for</typeparam>
        /// <returns>Byte offset of this element</returns>
        template<typename T>
        size_t ResolveOffset() const
        {
            assert(TypeRegistry::GetElementType<T>() == type);
            return GetOffset();
        }

    private:
        /// <summary>Returns a static empty element for error cases</summary>
        static LayoutElement& GetEmptyElement();

        /// <summary>Advances offset to the next 16-byte boundary (HLSL requirement)</summary>
        static size_t AdvanceToBoundary(size_t offset);

        /// <summary>Checks if placing an element at offset would cross a 16-byte boundary</summary>
        static bool CrossesBoundary(size_t offset, size_t size);

        /// <summary>Advances offset if placement would cross a 16-byte boundary</summary>
        static size_t AdvanceIfCrossesBoundary(size_t offset, size_t size);

        /// <summary>Validates that a name is a valid C++ identifier</summary>
        static bool IsValidSymbolName(const std::string& name);

        /// <summary>Finalizes layout for struct type elements</summary>
        size_t FinalizeStruct(size_t startOffset);

        /// <summary>Finalizes layout for array type elements</summary>
        size_t FinalizeArray(size_t startOffset);

        /// <summary>Generates signature string for struct elements</summary>
        std::string GetStructSignature() const;

        /// <summary>Generates signature string for array elements</summary>
        std::string GetArraySignature() const;

        ElementType type = ElementType::Empty;  ///< The type of this element
        std::optional<size_t> offset;           ///< Byte offset within parent (set during finalization)

        // Struct data
        /// <summary>Storage for struct members as (name, element) pairs</summary>
        std::vector<std::pair<std::string, LayoutElement>> members;

        // Array data
        /// <summary>Pointer to the element type of array elements</summary>
        std::unique_ptr<LayoutElement> arrayElementType;
        /// <summary>Number of elements in the array</summary>
        size_t arraySize = 0;
    };

    /// <summary>
    /// Base class for constant buffer layouts. Provides common functionality
    /// for accessing layout size and signature. Contains the root LayoutElement
    /// that defines the entire constant buffer structure.
    /// </summary>
    class Layout
    {
    public:
        /// <summary>Gets the total size in bytes of this layout (including all padding)</summary>
        /// <returns>Total size in bytes</returns>
        size_t GetSizeInBytes() const;

        /// <summary>Gets the unique signature string for this layout used in caching</summary>
        /// <returns>Layout signature string</returns>
        std::string GetSignature() const;

    protected:
        /// <summary>Protected constructor - only derived classes can create Layout objects</summary>
        /// <param name="root">Shared pointer to the root LayoutElement</param>
        Layout(std::shared_ptr<LayoutElement> root);

        /// <summary>Root element containing the entire layout structure</summary>
        std::shared_ptr<LayoutElement> rootElement;
    };

    /// <summary>
    /// Builder class for constructing constant buffer layouts dynamically.
    /// Allows adding members with different types and names before finalizing
    /// the layout. Once built, the layout can be cached and reused.
    /// </summary>
    class LayoutBuilder : public Layout
    {
    public:
        /// <summary>Creates a new empty layout builder ready for member addition</summary>
        LayoutBuilder();

        /// <summary>Accesses a member by name for further configuration</summary>
        /// <param name="name">Name of the member to access</param>
        /// <returns>Reference to the member element</returns>
        LayoutElement& operator[](const std::string& name);

        /// <summary>
        /// Template method to add a new member of the specified type.
        /// Provides compile-time type safety and convenience.
        /// </summary>
        /// <typeparam name="Type">ElementType to add</typeparam>
        /// <param name="name">Name of the member (must be valid C++ identifier)</param>
        /// <returns>Reference to the newly added member for method chaining</returns>
        template<ElementType Type>
        LayoutElement& Add(const std::string& name)
        {
            return rootElement->AddMember(Type, name);
        }

    private:
        friend class LayoutCache;
        friend class ConstantBufferData;

        /// <summary>Resets this builder to empty state for reuse</summary>
        void Reset();

        /// <summary>Extracts the root element, leaving this builder in empty state</summary>
        /// <returns>Shared pointer to the root element</returns>
        std::shared_ptr<LayoutElement> ExtractRoot();
    };

    /// <summary>
    /// Immutable layout that has been finalized with calculated offsets.
    /// Created by LayoutCache when a layout is resolved. Provides read-only
    /// access to the layout structure and is safe to share between multiple
    /// ConstantBufferData instances.
    /// </summary>
    class FinalizedLayout : public Layout
    {
    public:
        /// <summary>Accesses a member by name (read-only)</summary>
        /// <param name="name">Name of the member to access</param>
        /// <returns>Const reference to the member element</returns>
        const LayoutElement& operator[](const std::string& name) const;

        /// <summary>Gets the root element of this finalized layout</summary>
        /// <returns>Shared pointer to the root LayoutElement</returns>
        std::shared_ptr<LayoutElement> GetRoot() const { return rootElement; }

    private:
        friend class LayoutCache;

        /// <summary>Private constructor - only LayoutCache can create finalized layouts</summary>
        /// <param name="root">Shared pointer to the finalized root element</param>
        FinalizedLayout(std::shared_ptr<LayoutElement> root);
    };

    // Forward declarations for proxy classes
    class ConstantBufferDataRef;
    class ConstantBufferDataConstRef;

    /// <summary>
    /// Container class that holds both the layout definition and the actual data bytes
    /// for a constant buffer. Provides type-safe access to the data through proxy
    /// objects that understand the layout structure. Handles proper memory allocation
    /// and alignment according to HLSL requirements.
    /// </summary>
    class ConstantBufferData
    {
    public:
        /// <summary>Creates constant buffer data from a layout builder (takes ownership)</summary>
        /// <param name="layout">LayoutBuilder to extract layout from</param>
        ConstantBufferData(LayoutBuilder&& layout);

        /// <summary>Creates constant buffer data from a finalized layout</summary>
        /// <param name="layout">FinalizedLayout to use</param>
        ConstantBufferData(const FinalizedLayout& layout);

        /// <summary>Copy constructor - performs deep copy of both layout and data</summary>
        ConstantBufferData(const ConstantBufferData& other);

        /// <summary>Move constructor - efficient transfer of resources</summary>
        ConstantBufferData(ConstantBufferData&& other) noexcept;

        /// <summary>Accesses a member by name for read/write operations</summary>
        /// <param name="name">Name of the member to access</param>
        /// <returns>Proxy object for type-safe data access</returns>
        ConstantBufferDataRef operator[](const std::string& name);

        /// <summary>Accesses a member by name for read-only operations</summary>
        /// <param name="name">Name of the member to access</param>
        /// <returns>Const proxy object for type-safe data access</returns>
        ConstantBufferDataConstRef operator[](const std::string& name) const;

        /// <summary>Gets raw pointer to the data bytes (for GPU upload)</summary>
        /// <returns>Pointer to the data buffer</returns>
        const char* GetData() const { return data.data(); }

        /// <summary>Gets the total size of the data buffer in bytes</summary>
        /// <returns>Size in bytes</returns>
        size_t GetSizeInBytes() const { return data.size(); }

        /// <summary>Gets the root layout element</summary>
        /// <returns>Reference to the root LayoutElement</returns>
        const LayoutElement& GetRootLayout() const { return *rootLayout; }

        /// <summary>Copies data from another ConstantBufferData (layouts must match)</summary>
        /// <param name="other">Source ConstantBufferData to copy from</param>
        void CopyFrom(const ConstantBufferData& other);

        /// <summary>Gets shared pointer to the layout root (for internal use)</summary>
        /// <returns>Shared pointer to the root LayoutElement</returns>
        std::shared_ptr<LayoutElement> GetLayoutRoot() const { return rootLayout; }

    private:
        /// <summary>Shared pointer to the root layout element</summary>
        std::shared_ptr<LayoutElement> rootLayout;
        /// <summary>Raw data buffer with proper size and alignment</summary>
        std::vector<char> data;
    };

    /// <summary>
    /// Read-only proxy class for accessing constant buffer data in a type-safe manner.
    /// Provides const access to data members while understanding the layout structure.
    /// Supports both struct member access and array indexing operations.
    /// </summary>
    class ConstantBufferDataConstRef
    {
    public:
        /// <summary>Checks if this reference points to a valid element</summary>
        /// <returns>True if the referenced element exists</returns>
        bool Exists() const;

        /// <summary>Accesses a struct member by name (const version)</summary>
        /// <param name="name">Name of the member to access</param>
        /// <returns>Const reference proxy to the member</returns>
        ConstantBufferDataConstRef operator[](const std::string& name) const;

        /// <summary>Accesses an array element by index (const version)</summary>
        /// <param name="index">Array index to access</param>
        /// <returns>Const reference proxy to the array element</returns>
        ConstantBufferDataConstRef operator[](size_t index) const;

        /// <summary>
        /// Implicit conversion operator for type-safe data access.
        /// Automatically converts the referenced data to the requested type
        /// with compile-time type checking.
        /// </summary>
        /// <typeparam name="T">C++ type to convert to</typeparam>
        /// <returns>Const reference to the data as type T</returns>
        template<typename T>
        operator const T&() const
        {
            return *reinterpret_cast<const T*>(dataPtr + currentOffset + element->ResolveOffset<T>());
        }

        /// <summary>
        /// Gets a typed pointer to the data for direct access.
        /// Useful when you need to pass the data to functions expecting pointers.
        /// </summary>
        /// <typeparam name="T">C++ type to get pointer for</typeparam>
        /// <returns>Const pointer to the data as type T</returns>
        template<typename T>
        const T* GetPointer() const
        {
            return reinterpret_cast<const T*>(dataPtr + currentOffset + element->ResolveOffset<T>());
        }

    private:
        friend class ConstantBufferData;
        friend class ConstantBufferDataRef;

        /// <summary>Private constructor - only created by ConstantBufferData</summary>
        ConstantBufferDataConstRef(const LayoutElement* element, const char* dataPtr, size_t offset);

        const LayoutElement* element;   ///< Pointer to the layout element being referenced
        const char* dataPtr;           ///< Pointer to the start of the data buffer
        size_t currentOffset;          ///< Current offset within the data buffer
    };

    /// <summary>
    /// Mutable proxy class for accessing and modifying constant buffer data in a type-safe manner.
    /// Provides read/write access to data members while understanding the layout structure.
    /// Supports both struct member access and array indexing operations, as well as assignment.
    /// </summary>
    class ConstantBufferDataRef
    {
    public:
        /// <summary>Implicit conversion to const reference proxy</summary>
        /// <returns>ConstantBufferDataConstRef for the same data</returns>
        operator ConstantBufferDataConstRef() const;

        /// <summary>Checks if this reference points to a valid element</summary>
        /// <returns>True if the referenced element exists</returns>
        bool Exists() const;

        /// <summary>Accesses a struct member by name (mutable version)</summary>
        /// <param name="name">Name of the member to access</param>
        /// <returns>Mutable reference proxy to the member</returns>
        ConstantBufferDataRef operator[](const std::string& name) const;

        /// <summary>Accesses an array element by index (mutable version)</summary>
        /// <param name="index">Array index to access</param>
        /// <returns>Mutable reference proxy to the array element</returns>
        ConstantBufferDataRef operator[](size_t index) const;

        /// <summary>
        /// Implicit conversion operator for type-safe data access.
        /// Automatically converts the referenced data to the requested type
        /// with compile-time type checking.
        /// </summary>
        /// <typeparam name="T">C++ type to convert to</typeparam>
        /// <returns>Mutable reference to the data as type T</returns>
        template<typename T>
        operator T&() const
        {
            return *reinterpret_cast<T*>(dataPtr + currentOffset + element->ResolveOffset<T>());
        }

        /// <summary>
        /// Assignment operator for setting data values in a type-safe manner.
        /// Automatically handles type conversion and validates type compatibility.
        /// </summary>
        /// <typeparam name="T">C++ type of the value being assigned</typeparam>
        /// <param name="value">Value to assign</param>
        /// <returns>Reference to the assigned data</returns>
        template<typename T>
        T& operator=(const T& value) const
        {
            return static_cast<T&>(*this) = value;
        }

        /// <summary>
        /// Safe assignment method that checks if the element exists before setting.
        /// Useful for optional or conditional data updates.
        /// </summary>
        /// <typeparam name="T">C++ type of the value being assigned</typeparam>
        /// <param name="value">Value to assign</param>
        /// <returns>True if assignment succeeded, false if element doesn't exist</returns>
        template<typename T>
        bool TrySet(const T& value)
        {
            if (Exists())
            {
                *this = value;
                return true;
            }
            return false;
        }

        /// <summary>
        /// Gets a typed pointer to the data for direct access.
        /// Useful when you need to pass the data to functions expecting pointers.
        /// </summary>
        /// <typeparam name="T">C++ type to get pointer for</typeparam>
        /// <returns>Mutable pointer to the data as type T</returns>
        template<typename T>
        T* GetPointer() const
        {
            return reinterpret_cast<T*>(dataPtr + currentOffset + element->ResolveOffset<T>());
        }

    private:
        friend class ConstantBufferData;

        /// <summary>Private constructor - only created by ConstantBufferData</summary>
        ConstantBufferDataRef(const LayoutElement* element, char* dataPtr, size_t offset);

        const LayoutElement* element;   ///< Pointer to the layout element being referenced
        char* dataPtr;                 ///< Pointer to the start of the data buffer
        size_t currentOffset;          ///< Current offset within the data buffer
    };
}