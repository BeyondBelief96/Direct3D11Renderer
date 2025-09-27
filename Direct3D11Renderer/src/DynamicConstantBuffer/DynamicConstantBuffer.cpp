#include "DynamicConstantBuffer/DynamicConstantBuffer.h"
#include <string>
#include <algorithm>
#include <cctype>

namespace D3
{
    // =====================================================================================
    // TypeRegistry Implementation
    // =====================================================================================

    /// <summary>
    /// Static lookup table that maps ElementType enum values to their corresponding TypeInfo.
    /// This table defines the size, alignment, and signature for each supported HLSL type.
    /// Sizes and alignments must match HLSL constant buffer packing rules.
    /// </summary>
    const std::unordered_map<ElementType, TypeInfo> TypeRegistry::typeMap = {
        { ElementType::Float,    { sizeof(float),           sizeof(float),           "F1" } },  // 4 bytes
        { ElementType::Float2,   { sizeof(dx::XMFLOAT2),    sizeof(dx::XMFLOAT2),    "F2" } },  // 8 bytes
        { ElementType::Float3,   { sizeof(dx::XMFLOAT3),    sizeof(dx::XMFLOAT3),    "F3" } },  // 12 bytes
        { ElementType::Float4,   { sizeof(dx::XMFLOAT4),    sizeof(dx::XMFLOAT4),    "F4" } },  // 16 bytes
        { ElementType::Matrix4x4,{ sizeof(dx::XMFLOAT4X4),  sizeof(dx::XMFLOAT4X4),  "M4" } },  // 64 bytes
        { ElementType::Bool,     { 4u,                      4u,                      "BL" } }   // HLSL bool is always 4 bytes
    };

    /// <summary>
    /// Retrieves type information for a given ElementType from the static lookup table.
    /// This function is used throughout the system to get consistent size and alignment data.
    /// </summary>
    /// <param name="type">The ElementType to look up</param>
    /// <returns>Reference to TypeInfo containing size, alignment, and signature</returns>
    const TypeInfo& TypeRegistry::GetTypeInfo(ElementType type)
    {
        auto it = typeMap.find(type);
        assert(it != typeMap.end() && "Invalid ElementType passed to GetTypeInfo");
        return it->second;
    }

    /// <summary>
    /// Checks if the given ElementType represents a concrete system type that has
    /// an entry in the type map. Returns false for container types (Struct, Array)
    /// and the Empty type.
    /// </summary>
    /// <param name="type">ElementType to validate</param>
    /// <returns>True if type is a concrete system type with known size/alignment</returns>
    bool TypeRegistry::IsValidSystemType(ElementType type)
    {
        return typeMap.find(type) != typeMap.end();
    }

    // Template specializations that map C++ types to ElementType enum values.
    // These provide compile-time type safety when working with the dynamic constant buffer system.
    /// <summary>Maps C++ float to ElementType::Float</summary>
    template<> ElementType TypeRegistry::GetElementType<float>() { return ElementType::Float; }
    /// <summary>Maps DirectX::XMFLOAT2 to ElementType::Float2</summary>
    template<> ElementType TypeRegistry::GetElementType<dx::XMFLOAT2>() { return ElementType::Float2; }
    /// <summary>Maps DirectX::XMFLOAT3 to ElementType::Float3</summary>
    template<> ElementType TypeRegistry::GetElementType<dx::XMFLOAT3>() { return ElementType::Float3; }
    /// <summary>Maps DirectX::XMFLOAT4 to ElementType::Float4</summary>
    template<> ElementType TypeRegistry::GetElementType<dx::XMFLOAT4>() { return ElementType::Float4; }
    /// <summary>Maps DirectX::XMFLOAT4X4 to ElementType::Matrix4x4</summary>
    template<> ElementType TypeRegistry::GetElementType<dx::XMFLOAT4X4>() { return ElementType::Matrix4x4; }
    /// <summary>Maps C++ bool to ElementType::Bool</summary>
    template<> ElementType TypeRegistry::GetElementType<bool>() { return ElementType::Bool; }

    // =====================================================================================
    // LayoutElement Implementation
    // =====================================================================================

    /// <summary>
    /// Constructs a LayoutElement with the specified type.
    /// The element starts unfinalized (no offset calculated) and must be finalized
    /// before use in a constant buffer.
    /// </summary>
    /// <param name="elementType">Type of element to create (must not be Empty)</param>
    LayoutElement::LayoutElement(ElementType elementType) : type(elementType)
    {
        assert(elementType != ElementType::Empty && "Cannot create LayoutElement with Empty type");
    }

    /// <summary>
    /// Copy constructor that performs deep copying of the layout structure.
    /// Recursively copies all nested elements including array element types and struct members.
    /// This ensures that modifications to the copy don't affect the original.
    /// </summary>
    /// <param name="other">LayoutElement to copy from</param>
    LayoutElement::LayoutElement(const LayoutElement& other)
        : type(other.type), offset(other.offset), members(other.members), arraySize(other.arraySize)
    {
        // Deep copy the array element type if it exists
        if (other.arrayElementType)
        {
            arrayElementType = std::make_unique<LayoutElement>(*other.arrayElementType);
        }
    }

    /// <summary>
    /// Copy assignment operator that performs deep copying of the layout structure.
    /// Handles self-assignment and ensures proper cleanup of existing resources
    /// before copying from the source element.
    /// </summary>
    /// <param name="other">LayoutElement to copy from</param>
    /// <returns>Reference to this element after assignment</returns>
    LayoutElement& LayoutElement::operator=(const LayoutElement& other)
    {
        if (this != &other)  // Guard against self-assignment
        {
            type = other.type;
            offset = other.offset;
            members = other.members;  // vector assignment handles deep copy of members
            arraySize = other.arraySize;

            // Deep copy or reset the array element type
            if (other.arrayElementType)
            {
                arrayElementType = std::make_unique<LayoutElement>(*other.arrayElementType);
            }
            else
            {
                arrayElementType.reset();
            }
        }
        return *this;
    }

    /// <summary>
    /// Gets the byte offset of this element within its parent structure.
    /// The element must have been finalized before calling this method.
    /// </summary>
    /// <returns>Byte offset from the start of the parent structure</returns>
    /// <exception cref="assert">Throws assertion if element hasn't been finalized</exception>
    size_t LayoutElement::GetOffset() const
    {
        assert(offset.has_value() && "Element must be finalized before accessing offset");
        return *offset;
    }

    /// <summary>
    /// Calculates the total size in bytes of this element including any padding.
    /// For primitive types, returns the size from TypeRegistry.
    /// For structs, calculates from first element offset to end of last element (with padding).
    /// For arrays, returns element size (with padding) multiplied by array count.
    /// </summary>
    /// <returns>Total size in bytes including padding</returns>
    size_t LayoutElement::GetSize() const
    {
        switch (type)
        {
        case ElementType::Float:
        case ElementType::Float2:
        case ElementType::Float3:
        case ElementType::Float4:
        case ElementType::Matrix4x4:
        case ElementType::Bool:
            // For primitive types, return size from type registry
            return TypeRegistry::GetTypeInfo(type).size;

        case ElementType::Struct:
        {
            if (members.empty()) return 0;
            // Struct size = from start of struct to end of last member (with padding to 16-byte boundary)
            const auto& lastMember = members.back().second;
            return AdvanceToBoundary(lastMember.GetOffset() + lastMember.GetSize()) - GetOffset();
        }
        case ElementType::Array:
        {
            if (!arrayElementType) return 0;
            // Array size = element size (padded to 16-byte boundary) * array count
            return AdvanceToBoundary(arrayElementType->GetSize()) * arraySize;
        }
        default:
            assert(false && "Invalid element type for size calculation");
            return 0;
        }
    }

    /// <summary>
    /// Generates a unique string signature for this element that can be used for
    /// layout caching and comparison. The signature includes the element type and
    /// structure information for complex types.
    /// </summary>
    /// <returns>Unique string signature for this element's layout</returns>
    std::string LayoutElement::GetSignature() const
    {
        switch (type)
        {
        case ElementType::Float:
        case ElementType::Float2:
        case ElementType::Float3:
        case ElementType::Float4:
        case ElementType::Matrix4x4:
        case ElementType::Bool:
            // For primitive types, use the signature from type registry
            return TypeRegistry::GetTypeInfo(type).signature;
        case ElementType::Struct:
            // For structs, generate signature including all members
            return GetStructSignature();
        case ElementType::Array:
            // For arrays, generate signature including element type and count
            return GetArraySignature();
        default:
            assert(false && "Invalid type for signature generation");
            return "???";
        }
    }

    /// <summary>
    /// Adds a new member to this struct element. The struct must be of type Struct,
    /// and the member name must be a valid C++ identifier and unique within the struct.
    /// </summary>
    /// <param name="memberType">ElementType of the member to add</param>
    /// <param name="name">Name of the member (must be valid C++ identifier)</param>
    /// <returns>Reference to this element for method chaining</returns>
    LayoutElement& LayoutElement::AddMember(ElementType memberType, const std::string& name)
    {
        assert(type == ElementType::Struct && "Can only add members to Struct elements");
        assert(IsValidSymbolName(name) && "Member name must be a valid C++ identifier");

        // Check for duplicate names to prevent layout conflicts
        for (const auto& member : members)
        {
            assert(member.first != name && "Duplicate member name in struct");
        }

        // Add the new member to the struct
        members.emplace_back(name, LayoutElement(memberType));
        return *this;
    }

    /// <summary>
    /// Accesses a struct member by name. If the member doesn't exist,
    /// returns a reference to an empty (invalid) element that can be checked
    /// with the Exists() method.
    /// </summary>
    /// <param name="name">Name of the member to access</param>
    /// <returns>Reference to the member element, or empty element if not found</returns>
    LayoutElement& LayoutElement::operator[](const std::string& name)
    {
        assert(type == ElementType::Struct && "Can only access members of Struct elements");

        // Linear search for the member by name
        for (auto& member : members)
        {
            if (member.first == name)
            {
                return member.second;
            }
        }

        // Return empty element if member not found
        return GetEmptyElement();
    }

    /// <summary>
    /// Const version of member access operator. Uses const_cast to reuse
    /// the non-const implementation while maintaining const-correctness.
    /// </summary>
    /// <param name="name">Name of the member to access</param>
    /// <returns>Const reference to the member element, or empty element if not found</returns>
    const LayoutElement& LayoutElement::operator[](const std::string& name) const
    {
        return const_cast<LayoutElement&>(*this)[name];
    }

    /// <summary>
    /// Configures this element as an array with the specified element type and count.
    /// This element must be of type Array before calling this method.
    /// </summary>
    /// <param name="elementType">Type of elements in the array</param>
    /// <param name="count">Number of elements in the array (must be > 0)</param>
    /// <returns>Reference to this element for method chaining</returns>
    LayoutElement& LayoutElement::SetArrayType(ElementType elementType, size_t count)
    {
        assert(type == ElementType::Array && "Can only set array type on Array elements");
        assert(count > 0 && "Array count must be greater than 0");

        arrayElementType = std::make_unique<LayoutElement>(elementType);
        arraySize = count;
        return *this;
    }

    /// <summary>
    /// Gets the element type of array elements for modification.
    /// This element must be an Array type and must have been configured with SetArrayType.
    /// </summary>
    /// <returns>Reference to the array's element type</returns>
    LayoutElement& LayoutElement::GetArrayElementType()
    {
        assert(type == ElementType::Array && "Can only get array element type from Array elements");
        assert(arrayElementType != nullptr && "Array element type not set - call SetArrayType first");
        return *arrayElementType;
    }

    /// <summary>
    /// Gets the element type of array elements for read-only access.
    /// This element must be an Array type and must have been configured with SetArrayType.
    /// </summary>
    /// <returns>Const reference to the array's element type</returns>
    const LayoutElement& LayoutElement::GetArrayElementType() const
    {
        assert(type == ElementType::Array && "Can only get array element type from Array elements");
        assert(arrayElementType != nullptr && "Array element type not set - call SetArrayType first");
        return *arrayElementType;
    }

    /// <summary>
    /// Gets the number of elements in this array.
    /// This element must be of type Array.
    /// </summary>
    /// <returns>Number of elements in the array</returns>
    size_t LayoutElement::GetArraySize() const
    {
        assert(type == ElementType::Array && "Can only get array size from Array elements");
        return arraySize;
    }

    /// <summary>
    /// Calculates the byte offset and element type for a specific array index.
    /// Used by proxy classes to access individual array elements. The element size
    /// is padded to 16-byte boundaries as required by HLSL constant buffer rules.
    /// </summary>
    /// <param name="baseOffset">Base offset of the array in the buffer</param>
    /// <param name="index">Array index to calculate offset for (must be < arraySize)</param>
    /// <returns>Pair of (calculated offset, pointer to element type)</returns>
    std::pair<size_t, const LayoutElement*> LayoutElement::CalculateArrayOffset(size_t baseOffset, size_t index) const
    {
        assert(type == ElementType::Array && "Can only calculate array offsets for Array elements");
        assert(index < arraySize && "Array index out of bounds");

        // Calculate element size with proper 16-byte boundary alignment
        const size_t elementSize = AdvanceToBoundary(arrayElementType->GetSize());
        return { baseOffset + elementSize * index, &*arrayElementType };
    }

    /// <summary>
    /// Finalizes this element's layout by calculating its offset and the offsets of all
    /// its children. Applies HLSL constant buffer packing rules including 16-byte boundary
    /// alignment and prevention of elements crossing boundaries.
    /// </summary>
    /// <param name="startOffset">Starting byte offset for this element</param>
    /// <returns>Next available offset after this element (for placing subsequent elements)</returns>
    size_t LayoutElement::Finalize(size_t startOffset)
    {
        switch (type)
        {
        case ElementType::Float:
        case ElementType::Float2:
        case ElementType::Float3:
        case ElementType::Float4:
        case ElementType::Matrix4x4:
        case ElementType::Bool:
        {
            // For primitive types, apply boundary crossing rules and set offset
            const auto& typeInfo = TypeRegistry::GetTypeInfo(type);
            offset = AdvanceIfCrossesBoundary(startOffset, typeInfo.size);
            return *offset + typeInfo.size;
        }
        case ElementType::Struct:
            // Delegate to struct-specific finalization
            return FinalizeStruct(startOffset);
        case ElementType::Array:
            // Delegate to array-specific finalization
            return FinalizeArray(startOffset);
        default:
            assert(false && "Invalid type for finalization");
            return 0;
        }
    }

    /// <summary>
    /// Returns a static empty element used for error cases when member access fails.
    /// This element has type Empty and will return false for Exists() checks.
    /// </summary>
    /// <returns>Reference to a static empty LayoutElement</returns>
    LayoutElement& LayoutElement::GetEmptyElement()
    {
        static LayoutElement empty;  // Defaults to ElementType::Empty
        return empty;
    }

    /// <summary>
    /// Advances an offset to the next 16-byte boundary as required by HLSL constant buffer rules.
    /// If the offset is already on a 16-byte boundary, it remains unchanged.
    /// Formula: offset + (16 - offset % 16) % 16
    /// </summary>
    /// <param name="currentOffset">Current byte offset</param>
    /// <returns>Offset advanced to next 16-byte boundary</returns>
    size_t LayoutElement::AdvanceToBoundary(size_t currentOffset)
    {
        return currentOffset + (16u - currentOffset % 16u) % 16u;
    }

    /// <summary>
    /// Checks if placing an element of the given size at the current offset would
    /// cross a 16-byte boundary, which is not allowed in HLSL constant buffers.
    /// Also returns true if the element size itself exceeds 16 bytes.
    /// </summary>
    /// <param name="currentOffset">Proposed starting offset for the element</param>
    /// <param name="size">Size of the element in bytes</param>
    /// <returns>True if placement would cross a boundary or element is too large</returns>
    bool LayoutElement::CrossesBoundary(size_t currentOffset, size_t size)
    {
        const auto end = currentOffset + size;
        const auto pageStart = currentOffset / 16u;  // Which 16-byte page does it start in?
        const auto pageEnd = end / 16u;              // Which 16-byte page does it end in?

        // Element crosses boundary if it spans multiple pages (and doesn't end exactly on boundary)
        // OR if the element is larger than 16 bytes
        return (pageStart != pageEnd && end % 16 != 0u) || size > 16u;
    }

    /// <summary>
    /// Conditionally advances an offset to the next 16-byte boundary if placing
    /// an element of the given size would cross a boundary. This is a key function
    /// for implementing HLSL constant buffer packing rules.
    /// </summary>
    /// <param name="currentOffset">Current offset being considered</param>
    /// <param name="size">Size of element to be placed</param>
    /// <returns>Original offset if no boundary crossing, otherwise next 16-byte boundary</returns>
    size_t LayoutElement::AdvanceIfCrossesBoundary(size_t currentOffset, size_t size)
    {
        return CrossesBoundary(currentOffset, size) ? AdvanceToBoundary(currentOffset) : currentOffset;
    }

    /// <summary>
    /// Validates that a string is a valid C++ identifier that can be used as a member name.
    /// Rules: non-empty, doesn't start with digit, contains only alphanumeric chars and underscores.
    /// </summary>
    /// <param name="name">String to validate</param>
    /// <returns>True if the name is a valid C++ identifier</returns>
    bool LayoutElement::IsValidSymbolName(const std::string& name)
    {
        return !name.empty() &&                          // Must not be empty
               !std::isdigit(name.front()) &&           // Must not start with digit
               std::all_of(name.begin(), name.end(), [](char c) {
                   return std::isalnum(c) || c == '_';   // Only alphanumeric and underscore allowed
               });
    }

    /// <summary>
    /// Finalizes layout for struct-type elements. Structs are always aligned to 16-byte boundaries,
    /// and each member is finalized recursively with proper offset calculations.
    /// </summary>
    /// <param name="startOffset">Starting offset for this struct</param>
    /// <returns>Next available offset after all struct members</returns>
    size_t LayoutElement::FinalizeStruct(size_t startOffset)
    {
        assert(!members.empty() && "Struct must have at least one member");

        // Structs always start on 16-byte boundaries
        offset = AdvanceToBoundary(startOffset);
        auto currentOffset = *offset;

        // Finalize each member in order, updating current offset
        for (auto& member : members)
        {
            currentOffset = member.second.Finalize(currentOffset);
        }

        return currentOffset;
    }

    /// <summary>
    /// Finalizes layout for array-type elements. Arrays start on 16-byte boundaries,
    /// and the element type is finalized to determine the stride between elements.
    /// </summary>
    /// <param name="startOffset">Starting offset for this array</param>
    /// <returns>Next available offset after the entire array</returns>
    size_t LayoutElement::FinalizeArray(size_t startOffset)
    {
        assert(arrayElementType != nullptr && arraySize > 0 && "Array must have valid element type and size");

        // Arrays always start on 16-byte boundaries
        offset = AdvanceToBoundary(startOffset);

        // Finalize the element type to determine its size and layout
        arrayElementType->Finalize(*offset);

        // Return the offset after the entire array
        return GetOffset() + GetSize();
    }

    /// <summary>
    /// Generates a unique signature string for struct-type elements.
    /// Format: "St{memberName:memberSignature;...}"
    /// Used for layout caching to identify identical struct layouts.
    /// </summary>
    /// <returns>Unique signature string for this struct layout</returns>
    std::string LayoutElement::GetStructSignature() const
    {
        using namespace std::string_literals;
        auto sig = "St{"s;

        // Include each member's name and signature
        for (const auto& member : members)
        {
            sig += member.first + ":"s + member.second.GetSignature() + ";"s;
        }

        sig += "}"s;
        return sig;
    }

    /// <summary>
    /// Generates a unique signature string for array-type elements.
    /// Format: "Ar:count{elementSignature}"
    /// Used for layout caching to identify identical array layouts.
    /// </summary>
    /// <returns>Unique signature string for this array layout</returns>
    std::string LayoutElement::GetArraySignature() const
    {
        using namespace std::string_literals;
        return "Ar:"s + std::to_string(arraySize) + "{"s + arrayElementType->GetSignature() + "}"s;
    }

    // =====================================================================================
    // Layout Implementation
    // =====================================================================================

    /// <summary>
    /// Protected constructor for Layout base class. Takes ownership of the root element
    /// and stores it for use by derived classes.
    /// </summary>
    /// <param name="root">Shared pointer to the root LayoutElement</param>
    Layout::Layout(std::shared_ptr<LayoutElement> root) : rootElement(std::move(root)) {}

    /// <summary>
    /// Gets the total size in bytes of this layout including all padding.
    /// Delegates to the root element's GetSize() method.
    /// </summary>
    /// <returns>Total layout size in bytes</returns>
    size_t Layout::GetSizeInBytes() const
    {
        return rootElement->GetSize();
    }

    /// <summary>
    /// Gets the unique signature string for this layout used in caching.
    /// Delegates to the root element's GetSignature() method.
    /// </summary>
    /// <returns>Unique layout signature string</returns>
    std::string Layout::GetSignature() const
    {
        return rootElement->GetSignature();
    }

    // =====================================================================================
    // LayoutBuilder Implementation
    // =====================================================================================

    /// <summary>
    /// Creates a new LayoutBuilder with an empty struct as the root element.
    /// The root is always a struct type to allow adding multiple top-level members.
    /// </summary>
    LayoutBuilder::LayoutBuilder() : Layout(std::make_shared<LayoutElement>(ElementType::Struct)) {}

    /// <summary>
    /// Accesses a top-level member by name for further configuration.
    /// Delegates to the root element's member access operator.
    /// </summary>
    /// <param name="name">Name of the member to access</param>
    /// <returns>Reference to the member element</returns>
    LayoutElement& LayoutBuilder::operator[](const std::string& name)
    {
        return (*rootElement)[name];
    }

    /// <summary>
    /// Resets this builder to an empty state with a new empty struct root.
    /// Used internally by the LayoutCache to reuse builder instances.
    /// </summary>
    void LayoutBuilder::Reset()
    {
        rootElement = std::make_shared<LayoutElement>(ElementType::Struct);
    }

    /// <summary>
    /// Extracts the root element from this builder, finalizing its layout and
    /// resetting the builder to empty state. This transfers ownership of the
    /// layout to the caller and prepares the builder for reuse.
    /// </summary>
    /// <returns>Finalized root element with calculated offsets</returns>
    std::shared_ptr<LayoutElement> LayoutBuilder::ExtractRoot()
    {
        auto temp = std::move(rootElement);
        temp->Finalize(0);  // Calculate all offsets starting from 0
        Reset();            // Prepare builder for reuse
        return temp;
    }

    // =====================================================================================
    // FinalizedLayout Implementation
    // =====================================================================================

    /// <summary>
    /// Private constructor for FinalizedLayout. Only LayoutCache can create instances
    /// to ensure proper finalization and caching behavior.
    /// </summary>
    /// <param name="root">Finalized root element with calculated offsets</param>
    FinalizedLayout::FinalizedLayout(std::shared_ptr<LayoutElement> root) : Layout(std::move(root)) {}

    /// <summary>
    /// Accesses a top-level member by name for read-only operations.
    /// Delegates to the root element's const member access operator.
    /// </summary>
    /// <param name="name">Name of the member to access</param>
    /// <returns>Const reference to the member element</returns>
    const LayoutElement& FinalizedLayout::operator[](const std::string& name) const
    {
        return (*rootElement)[name];
    }

    // =====================================================================================
    // ConstantBufferData Implementation
    // =====================================================================================

    /// <summary>
    /// Creates ConstantBufferData from a LayoutBuilder, taking ownership of the layout
    /// and allocating appropriately sized data buffer. The builder is reset after extraction.
    /// </summary>
    /// <param name="layout">LayoutBuilder to extract layout from (passed by rvalue reference)</param>
    ConstantBufferData::ConstantBufferData(LayoutBuilder&& layout)
        : rootLayout(layout.ExtractRoot()), data(rootLayout->GetSize())
    {}

    /// <summary>
    /// Creates ConstantBufferData from a FinalizedLayout, sharing the layout
    /// and allocating appropriately sized data buffer.
    /// </summary>
    /// <param name="layout">FinalizedLayout to use (layout is shared, not copied)</param>
    ConstantBufferData::ConstantBufferData(const FinalizedLayout& layout)
        : rootLayout(layout.GetRoot()), data(rootLayout->GetSize())
    {}

    /// <summary>
    /// Copy constructor that shares the layout but performs deep copy of the data buffer.
    /// This allows multiple ConstantBufferData instances to use the same layout definition
    /// while maintaining independent data.
    /// </summary>
    /// <param name="other">ConstantBufferData to copy from</param>
    ConstantBufferData::ConstantBufferData(const ConstantBufferData& other)
        : rootLayout(other.rootLayout), data(other.data)  // Layout shared, data copied
    {}

    /// <summary>
    /// Move constructor for efficient transfer of resources. Both layout and data
    /// are moved, leaving the source object in a valid but unspecified state.
    /// </summary>
    /// <param name="other">ConstantBufferData to move from</param>
    ConstantBufferData::ConstantBufferData(ConstantBufferData&& other) noexcept
        : rootLayout(std::move(other.rootLayout)), data(std::move(other.data))
    {}

    /// <summary>
    /// Accesses a top-level member by name for read/write operations.
    /// Returns a proxy object that provides type-safe access to the data.
    /// </summary>
    /// <param name="name">Name of the member to access</param>
    /// <returns>Mutable proxy object for type-safe data access</returns>
    ConstantBufferDataRef ConstantBufferData::operator[](const std::string& name)
    {
        return { &(*rootLayout)[name], data.data(), 0u };
    }

    /// <summary>
    /// Accesses a top-level member by name for read-only operations.
    /// Returns a const proxy object that provides type-safe read access to the data.
    /// </summary>
    /// <param name="name">Name of the member to access</param>
    /// <returns>Const proxy object for type-safe data access</returns>
    ConstantBufferDataConstRef ConstantBufferData::operator[](const std::string& name) const
    {
        return { &(*rootLayout)[name], data.data(), 0u };
    }

    /// <summary>
    /// Copies data from another ConstantBufferData instance. Both instances must
    /// use the exact same layout (verified by pointer comparison) to ensure
    /// data compatibility and prevent corruption.
    /// </summary>
    /// <param name="other">Source ConstantBufferData to copy data from</param>
    void ConstantBufferData::CopyFrom(const ConstantBufferData& other)
    {
        assert(rootLayout.get() == other.rootLayout.get() && "Cannot copy between different layouts");
        std::copy(other.data.begin(), other.data.end(), data.begin());
    }

    // =====================================================================================
    // ConstantBufferDataConstRef Implementation
    // =====================================================================================

    /// <summary>
    /// Private constructor for ConstantBufferDataConstRef. Only ConstantBufferData
    /// and ConstantBufferDataRef can create instances to maintain data integrity.
    /// </summary>
    /// <param name="element">Pointer to the LayoutElement being referenced</param>
    /// <param name="dataPtr">Pointer to the start of the data buffer</param>
    /// <param name="offset">Current offset within the data buffer</param>
    ConstantBufferDataConstRef::ConstantBufferDataConstRef(const LayoutElement* element, const char* dataPtr, size_t offset)
        : element(element), dataPtr(dataPtr), currentOffset(offset)
    {}

    /// <summary>
    /// Checks if this reference points to a valid element in the layout.
    /// Returns false if the referenced element was not found during access.
    /// </summary>
    /// <returns>True if the referenced element exists and is valid</returns>
    bool ConstantBufferDataConstRef::Exists() const
    {
        return element->Exists();
    }

    /// <summary>
    /// Accesses a struct member by name, creating a new const reference proxy
    /// that points to the member's data within the same buffer.
    /// </summary>
    /// <param name="name">Name of the struct member to access</param>
    /// <returns>Const reference proxy to the member</returns>
    ConstantBufferDataConstRef ConstantBufferDataConstRef::operator[](const std::string& name) const
    {
        return { &(*element)[name], dataPtr, currentOffset };
    }

    /// <summary>
    /// Accesses an array element by index, creating a new const reference proxy
    /// that points to the specific array element's data.
    /// </summary>
    /// <param name="index">Array index to access (must be < array size)</param>
    /// <returns>Const reference proxy to the array element</returns>
    ConstantBufferDataConstRef ConstantBufferDataConstRef::operator[](size_t index) const
    {
        const auto offsetData = element->CalculateArrayOffset(currentOffset, index);
        return { offsetData.second, dataPtr, offsetData.first };
    }

    // =====================================================================================
    // ConstantBufferDataRef Implementation
    // =====================================================================================

    /// <summary>
    /// Private constructor for ConstantBufferDataRef. Only ConstantBufferData
    /// can create instances to maintain data integrity and control access.
    /// </summary>
    /// <param name="element">Pointer to the LayoutElement being referenced</param>
    /// <param name="dataPtr">Pointer to the start of the mutable data buffer</param>
    /// <param name="offset">Current offset within the data buffer</param>
    ConstantBufferDataRef::ConstantBufferDataRef(const LayoutElement* element, char* dataPtr, size_t offset)
        : element(element), dataPtr(dataPtr), currentOffset(offset)
    {}

    /// <summary>
    /// Implicit conversion to const reference proxy. Allows mutable references
    /// to be used in contexts that expect const references, providing natural
    /// const-correctness behavior.
    /// </summary>
    /// <returns>ConstantBufferDataConstRef pointing to the same data</returns>
    ConstantBufferDataRef::operator ConstantBufferDataConstRef() const
    {
        return { element, dataPtr, currentOffset };
    }

    /// <summary>
    /// Checks if this reference points to a valid element in the layout.
    /// Returns false if the referenced element was not found during access.
    /// </summary>
    /// <returns>True if the referenced element exists and is valid</returns>
    bool ConstantBufferDataRef::Exists() const
    {
        return element->Exists();
    }

    /// <summary>
    /// Accesses a struct member by name, creating a new mutable reference proxy
    /// that points to the member's data within the same buffer.
    /// </summary>
    /// <param name="name">Name of the struct member to access</param>
    /// <returns>Mutable reference proxy to the member</returns>
    ConstantBufferDataRef ConstantBufferDataRef::operator[](const std::string& name) const
    {
        return { &(*element)[name], dataPtr, currentOffset };
    }

    /// <summary>
    /// Accesses an array element by index, creating a new mutable reference proxy
    /// that points to the specific array element's data.
    /// </summary>
    /// <param name="index">Array index to access (must be < array size)</param>
    /// <returns>Mutable reference proxy to the array element</returns>
    ConstantBufferDataRef ConstantBufferDataRef::operator[](size_t index) const
    {
        const auto offsetData = element->CalculateArrayOffset(currentOffset, index);
        return { offsetData.second, dataPtr, offsetData.first };
    }
}