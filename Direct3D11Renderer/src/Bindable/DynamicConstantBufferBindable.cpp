#include "Bindable/DynamicConstantBufferBindable.h"
#include "Core/Graphics.h"

// =====================================================================================
// DynamicPixelConstantBufferBindable Implementation
// =====================================================================================
/// <summary>
/// Creates a DirectX 11 constant buffer with dynamic usage for CPU updates.
/// The buffer size is determined by the layout root element, and optional initial
/// data can be provided for immediate upload.
///
/// Buffer Configuration:
/// - Usage: D3D11_USAGE_DYNAMIC (allows CPU writes)
/// - CPUAccessFlags: D3D11_CPU_ACCESS_WRITE (CPU can map for writing)
/// - BindFlags: D3D11_BIND_CONSTANT_BUFFER (can be bound as constant buffer)
/// </summary>
/// <param name="gfx">Graphics context for DirectX operations</param>
/// <param name="layoutRoot">Root layout element defining buffer size and structure</param>
/// <param name="slot">Pixel shader slot number for binding</param>
/// <param name="pBuffer">Optional initial data to upload to GPU</param>
DynamicPixelConstantBufferBindable::DynamicPixelConstantBufferBindable(Graphics& gfx, const D3::LayoutElement& layoutRoot, UINT slot, const D3::ConstantBufferData* pBuffer)
    : slot(slot)
{
    DEBUGMANAGER(gfx);

    // Configure buffer description for dynamic constant buffer
    D3D11_BUFFER_DESC bufferDesc{};
    bufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;  // Can be bound as constant buffer
    bufferDesc.Usage = D3D11_USAGE_DYNAMIC;             // Allows CPU updates
    bufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;  // CPU can write to buffer
    bufferDesc.MiscFlags = 0u;                          // No special flags
    bufferDesc.ByteWidth = static_cast<UINT>(layoutRoot.GetSize());  // Size from layout
    bufferDesc.StructureByteStride = 0u;                // Not a structured buffer

    // Create buffer with or without initial data
    if (pBuffer != nullptr)
    {
        // Initialize buffer with provided data
        D3D11_SUBRESOURCE_DATA subresourceData{};
        subresourceData.pSysMem = pBuffer->GetData();
        GFX_THROW_INFO(GetDevice(gfx)->CreateBuffer(&bufferDesc, &subresourceData, &pConstantBuffer));
    }
    else
    {
        // Create empty buffer (will be filled later via Update())
        GFX_THROW_INFO(GetDevice(gfx)->CreateBuffer(&bufferDesc, nullptr, &pConstantBuffer));
    }
}

/// <summary>
/// Updates the GPU constant buffer with new data using dynamic mapping.
/// Validates that the source buffer layout matches this bindable's layout
/// before performing the upload.
///
/// Uses D3D11_MAP_WRITE_DISCARD for optimal performance:
/// - Discards previous buffer contents
/// - Allows GPU to allocate new memory if current buffer is in use
/// - Avoids pipeline stalls from GPU/CPU synchronization
/// </summary>
/// <param name="gfx">Graphics context for DirectX operations</param>
/// <param name="buffer">Source data to upload (layout must match)</param>
void DynamicPixelConstantBufferBindable::Update(Graphics& gfx, const D3::ConstantBufferData& buffer)
{
    // Validate that layouts match - prevents data corruption
    assert(&buffer.GetRootLayout() == &GetRootLayoutElement() && "Buffer layout mismatch");
    DEBUGMANAGER(gfx);

    // Map buffer for CPU write access
    D3D11_MAPPED_SUBRESOURCE mappedResource;
    GFX_THROW_INFO(GetContext(gfx)->Map(pConstantBuffer.Get(), 0u, D3D11_MAP_WRITE_DISCARD, 0u, &mappedResource));

    // Copy data from CPU buffer to GPU buffer
    memcpy(mappedResource.pData, buffer.GetData(), buffer.GetSizeInBytes());

    // Unmap to make data available to GPU
    GetContext(gfx)->Unmap(pConstantBuffer.Get(), 0u);
}

/// <summary>
/// Binds this constant buffer to the pixel shader stage at the configured slot.
/// This makes the buffer data available to pixel shaders via the specified slot number.
/// </summary>
/// <param name="gfx">Graphics context for DirectX operations</param>
void DynamicPixelConstantBufferBindable::Bind(Graphics& gfx) noexcept
{
    GetContext(gfx)->PSSetConstantBuffers(slot, 1u, pConstantBuffer.GetAddressOf());
}

// =====================================================================================
// CachingDynamicPixelConstantBufferBindable Implementation
// =====================================================================================

/// <summary>
/// Creates a caching constant buffer bindable from a finalized layout.
/// Initializes the GPU buffer without data and creates a CPU-side buffer for caching.
/// </summary>
/// <param name="gfx">Graphics context for DirectX operations</param>
/// <param name="layout">Finalized layout defining buffer structure</param>
/// <param name="slot">Pixel shader slot number</param>
CachingDynamicPixelConstantBufferBindable::CachingDynamicPixelConstantBufferBindable(Graphics& gfx, const D3::FinalizedLayout& layout, UINT slot)
    : DynamicPixelConstantBufferBindable(gfx, *layout.GetRoot(), slot, nullptr), buffer(layout)
{}

/// <summary>
/// Creates a caching constant buffer bindable from existing buffer data.
/// Initializes both the GPU buffer and CPU cache with the provided data.
/// </summary>
/// <param name="gfx">Graphics context for DirectX operations</param>
/// <param name="bufferData">Source data for initialization</param>
/// <param name="slot">Pixel shader slot number</param>
CachingDynamicPixelConstantBufferBindable::CachingDynamicPixelConstantBufferBindable(Graphics& gfx, const D3::ConstantBufferData& bufferData, UINT slot)
    : DynamicPixelConstantBufferBindable(gfx, bufferData.GetRootLayout(), slot, &bufferData), buffer(bufferData)
{}

/// <summary>
/// Returns the root layout element from the cached buffer.
/// Used for layout validation and size calculations.
/// </summary>
/// <returns>Reference to the root layout element</returns>
const D3::LayoutElement& CachingDynamicPixelConstantBufferBindable::GetRootLayoutElement() const noexcept
{
    return buffer.GetRootLayout();
}

/// <summary>
/// Provides read-only access to the cached constant buffer data.
/// Useful for inspecting current values or creating copies.
/// </summary>
/// <returns>Const reference to the cached buffer data</returns>
const D3::ConstantBufferData& CachingDynamicPixelConstantBufferBindable::GetBuffer() const noexcept
{
    return buffer;
}

/// <summary>
/// Updates the cached buffer data and marks it as dirty for next GPU upload.
/// The new data must have the same layout as the existing buffer to prevent corruption.
/// </summary>
/// <param name="bufferData">New buffer data to copy (layout must match)</param>
void CachingDynamicPixelConstantBufferBindable::SetBuffer(const D3::ConstantBufferData& bufferData)
{
    buffer.CopyFrom(bufferData);  // CopyFrom validates layout compatibility
    isDirty = true;               // Mark for upload on next bind
}

/// <summary>
/// Binds the constant buffer, uploading dirty data to GPU if necessary.
/// Implements lazy evaluation - data is only uploaded when dirty and binding is requested.
/// This minimizes GPU bandwidth usage and improves performance.
/// </summary>
/// <param name="gfx">Graphics context for DirectX operations</param>
void CachingDynamicPixelConstantBufferBindable::Bind(Graphics& gfx) noexcept
{
    if (isDirty)
    {
        Update(gfx, buffer);  // Upload cached data to GPU
        isDirty = false;      // Clear dirty flag
    }
    DynamicPixelConstantBufferBindable::Bind(gfx);  // Bind buffer to pipeline
}

void CachingDynamicPixelConstantBufferBindable::Accept(TechniqueProbe& probe)
{
    if (probe.VisitBuffer(buffer))
    {
        isDirty = true;
    }
}

/// <summary>
/// Creates or retrieves a cached instance from the BindableCache based on layout and slot.
/// Enables sharing of identical bindables across multiple objects for memory efficiency.
/// </summary>
/// <param name="gfx">Graphics context for potential creation</param>
/// <param name="layout">Layout for the bindable</param>
/// <param name="slot">Pixel shader slot number</param>
/// <returns>Shared pointer to the bindable (may be newly created or cached)</returns>
std::shared_ptr<CachingDynamicPixelConstantBufferBindable> CachingDynamicPixelConstantBufferBindable::Resolve(Graphics& gfx, const D3::FinalizedLayout& layout, UINT slot)
{
    return BindableCache::Resolve<CachingDynamicPixelConstantBufferBindable>(gfx, layout, slot);
}

/// <summary>
/// Creates or retrieves a cached instance from the BindableCache based on buffer data and slot.
/// The layout is extracted from the buffer data for cache key generation.
/// </summary>
/// <param name="gfx">Graphics context for potential creation</param>
/// <param name="buffer">Buffer data containing layout and initial values</param>
/// <param name="slot">Pixel shader slot number</param>
/// <returns>Shared pointer to the bindable (may be newly created or cached)</returns>
std::shared_ptr<CachingDynamicPixelConstantBufferBindable> CachingDynamicPixelConstantBufferBindable::Resolve(Graphics& gfx, const D3::ConstantBufferData& buffer, UINT slot)
{
    return BindableCache::Resolve<CachingDynamicPixelConstantBufferBindable>(gfx, buffer, slot);
}

/// <summary>
/// Generates a unique identifier for BindableCache based on layout signature and slot.
/// The UID combines the class type, layout signature, and slot number to ensure uniqueness.
/// </summary>
/// <param name="layout">Layout to include in the UID</param>
/// <param name="slot">Slot number to include in the UID</param>
/// <returns>Unique identifier string for cache lookup</returns>
std::string CachingDynamicPixelConstantBufferBindable::GenerateUID(const D3::FinalizedLayout& layout, UINT slot)
{
    using namespace std::string_literals;
    return typeid(CachingDynamicPixelConstantBufferBindable).name() + "#"s + layout.GetSignature() + "#"s + std::to_string(slot);
}

/// <summary>
/// Generates a unique identifier for BindableCache based on buffer layout and slot.
/// Extracts the layout signature from the buffer data for UID generation.
/// </summary>
/// <param name="buffer">Buffer data containing layout information</param>
/// <param name="slot">Slot number to include in the UID</param>
/// <returns>Unique identifier string for cache lookup</returns>
std::string CachingDynamicPixelConstantBufferBindable::GenerateUID(const D3::ConstantBufferData& buffer, UINT slot)
{
    using namespace std::string_literals;
    return typeid(CachingDynamicPixelConstantBufferBindable).name() + "#"s + buffer.GetRootLayout().GetSignature() + "#"s + std::to_string(slot);
}

/// <summary>
/// Returns the unique identifier for this bindable instance.
/// Used by BindableCache for resource management and lookup.
/// </summary>
/// <returns>Unique identifier string for this instance</returns>
std::string CachingDynamicPixelConstantBufferBindable::GetUID() const noexcept
{
    using namespace std::string_literals;
    return typeid(CachingDynamicPixelConstantBufferBindable).name() + "#"s + buffer.GetRootLayout().GetSignature() + "#"s + std::to_string(slot);
}

// =====================================================================================
// NoCacheDynamicPixelConstantBufferBindable Implementation
// =====================================================================================

/// <summary>
/// Creates a non-caching constant buffer bindable from a finalized layout.
/// Stores only a reference to the layout without maintaining CPU-side data cache.
/// </summary>
/// <param name="gfx">Graphics context for DirectX operations</param>
/// <param name="layout">Finalized layout defining buffer structure</param>
/// <param name="slot">Pixel shader slot number</param>
NoCacheDynamicPixelConstantBufferBindable::NoCacheDynamicPixelConstantBufferBindable(Graphics& gfx, const D3::FinalizedLayout& layout, UINT slot)
    : DynamicPixelConstantBufferBindable(gfx, *layout.GetRoot(), slot, nullptr), pLayoutRoot(layout.GetRoot())
{}

/// <summary>
/// Creates a non-caching constant buffer bindable from existing buffer data.
/// Initializes the GPU buffer with the provided data but does not retain a CPU copy.
/// </summary>
/// <param name="gfx">Graphics context for DirectX operations</param>
/// <param name="buffer">Source data for initial GPU upload</param>
/// <param name="slot">Pixel shader slot number</param>
NoCacheDynamicPixelConstantBufferBindable::NoCacheDynamicPixelConstantBufferBindable(Graphics& gfx, const D3::ConstantBufferData& buffer, UINT slot)
    : DynamicPixelConstantBufferBindable(gfx, buffer.GetRootLayout(), slot, &buffer), pLayoutRoot(buffer.GetLayoutRoot())
{}

/// <summary>
/// Returns the root layout element for this buffer.
/// Used for layout validation and size calculations without maintaining a data cache.
/// </summary>
/// <returns>Reference to the root layout element</returns>
const D3::LayoutElement& NoCacheDynamicPixelConstantBufferBindable::GetRootLayoutElement() const noexcept
{
    return *pLayoutRoot;
}