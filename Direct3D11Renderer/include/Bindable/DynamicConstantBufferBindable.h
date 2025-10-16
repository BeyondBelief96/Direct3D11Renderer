#pragma once

#include "Bindable.h"
#include "BindableCache.h"
#include "Exceptions/GraphicsExceptions.h"
#include "DynamicConstantBuffer/DynamicConstantBuffer.h"
#include "DynamicConstantBuffer/LayoutCache.h"
#include "RenderPass/TechniqueProbe.h"
#include <wrl.h>
#include <memory>

/// <file>
/// Dynamic Constant Buffer Bindable Classes
///
/// This file defines GPU bindable wrappers for the dynamic constant buffer system.
/// These classes bridge the gap between the CPU-side dynamic constant buffer
/// layout system and the GPU-side DirectX 11 constant buffer resources.
///
/// The bindable classes handle:
/// - DirectX 11 buffer creation with proper size and usage flags
/// - Efficient GPU data uploads using dynamic mapping
/// - Integration with the bindable cache system for resource sharing
/// - Automatic dirty tracking to minimize GPU uploads
/// </file>

/// <summary>
/// Abstract base class for dynamic constant buffer bindables that bind to pixel shader slots.
/// Provides common functionality for DirectX 11 buffer management, data uploading, and
/// GPU binding. Derived classes must implement GetRootLayoutElement() to provide layout information.
///
/// This class handles the low-level DirectX 11 operations:
/// - Creating dynamic constant buffers with D3D11_USAGE_DYNAMIC
/// - Mapping/unmapping buffers for CPU writes
/// - Binding buffers to the pixel shader pipeline stage
/// </summary>
class DynamicPixelConstantBufferBindable : public Bindable
{
public:
    /// <summary>
    /// Updates the GPU buffer with new data from a ConstantBufferData instance.
    /// Performs validation to ensure the buffer layout matches the bindable's layout.
    /// Uses D3D11_MAP_WRITE_DISCARD for optimal performance.
    /// </summary>
    /// <param name="gfx">Graphics context for DirectX operations</param>
    /// <param name="buffer">Source data to upload (layout must match this bindable's layout)</param>
    void Update(Graphics& gfx, const D3::ConstantBufferData& buffer);

    /// <summary>
    /// Binds this constant buffer to the pixel shader stage at the specified slot.
    /// Called during the rendering pipeline to make the buffer available to shaders.
    /// </summary>
    /// <param name="gfx">Graphics context for DirectX operations</param>
    void Bind(Graphics& gfx) noexcept override;

    /// <summary>
    /// Pure virtual function that derived classes must implement to provide
    /// access to the root layout element for validation and size calculations.
    /// </summary>
    /// <returns>Reference to the root LayoutElement defining this buffer's structure</returns>
    virtual const D3::LayoutElement& GetRootLayoutElement() const noexcept = 0;

protected:
    /// <summary>
    /// Protected constructor for use by derived classes. Creates the DirectX 11 buffer
    /// with appropriate size and flags based on the layout root element.
    /// </summary>
    /// <param name="gfx">Graphics context for buffer creation</param>
    /// <param name="layoutRoot">Root layout element defining buffer structure and size</param>
    /// <param name="slot">Pixel shader slot number for binding</param>
    /// <param name="pBuffer">Optional initial data to upload to the buffer</param>
    DynamicPixelConstantBufferBindable(Graphics& gfx, const D3::LayoutElement& layoutRoot, UINT slot, const D3::ConstantBufferData* pBuffer = nullptr);

    /// <summary>DirectX 11 constant buffer resource</summary>
    Microsoft::WRL::ComPtr<ID3D11Buffer> pConstantBuffer;
    /// <summary>Pixel shader slot number for binding</summary>
    UINT slot;
};

/// <summary>
/// Caching version of dynamic constant buffer bindable that maintains its own data buffer
/// and implements dirty tracking for optimal GPU upload performance. This class is suitable
/// for scenarios where the constant buffer data changes frequently and needs to be cached
/// on the CPU side.
///
/// Key features:
/// - Maintains a CPU-side copy of the constant buffer data
/// - Implements dirty tracking to minimize GPU uploads
/// - Integrates with BindableCache for resource sharing
/// - Supports both layout-based and data-based construction
///
/// Use this class when:
/// - You need to frequently update constant buffer data
/// - Multiple objects might share the same buffer layout
/// - You want automatic dirty tracking and efficient GPU uploads
/// </summary>
class CachingDynamicPixelConstantBufferBindable : public DynamicPixelConstantBufferBindable
{
public:
    /// <summary>
    /// Creates a caching constant buffer bindable from a finalized layout.
    /// The buffer data is initialized to zero.
    /// </summary>
    /// <param name="gfx">Graphics context for buffer creation</param>
    /// <param name="layout">Finalized layout defining buffer structure</param>
    /// <param name="slot">Pixel shader slot number for binding</param>
    CachingDynamicPixelConstantBufferBindable(Graphics& gfx, const D3::FinalizedLayout& layout, UINT slot);

    /// <summary>
    /// Creates a caching constant buffer bindable from existing buffer data.
    /// The GPU buffer is initialized with the provided data.
    /// </summary>
    /// <param name="gfx">Graphics context for buffer creation</param>
    /// <param name="buffer">Source data and layout for the constant buffer</param>
    /// <param name="slot">Pixel shader slot number for binding</param>
    CachingDynamicPixelConstantBufferBindable(Graphics& gfx, const D3::ConstantBufferData& buffer, UINT slot);

    /// <summary>Returns the root layout element for this buffer</summary>
    const D3::LayoutElement& GetRootLayoutElement() const noexcept override;

    /// <summary>Gets read-only access to the cached buffer data</summary>
    /// <returns>Reference to the internal ConstantBufferData</returns>
    const D3::ConstantBufferData& GetBuffer() const noexcept;

    /// <summary>
    /// Updates the cached buffer data and marks it as dirty for the next GPU upload.
    /// The new buffer data must have the same layout as the existing buffer.
    /// </summary>
    /// <param name="buffer">New buffer data to copy (layout must match)</param>
    void SetBuffer(const D3::ConstantBufferData& buffer);

    /// <summary>
    /// Binds the constant buffer, uploading dirty data to GPU if necessary.
    /// Implements lazy upload strategy - data is only uploaded when dirty and binding is requested.
    /// </summary>
    /// <param name="gfx">Graphics context for operations</param>
    void Bind(Graphics& gfx) noexcept override;

    /// <summary>
    /// Accepts a TechniqueProbe object, typically for visitor pattern operations.
    /// </summary>
    /// <param name="probe">A reference to a TechniqueProbe object to be accepted.</param>
    void Accept(TechniqueProbe& probe) override;

    /// <summary>Creates or retrieves a cached bindable from the BindableCache using a layout</summary>
    static std::shared_ptr<CachingDynamicPixelConstantBufferBindable> Resolve(Graphics& gfx, const D3::FinalizedLayout& layout, UINT slot = 1);

    /// <summary>Creates or retrieves a cached bindable from the BindableCache using buffer data</summary>
    static std::shared_ptr<CachingDynamicPixelConstantBufferBindable> Resolve(Graphics& gfx, const D3::ConstantBufferData& buffer, UINT slot = 1);

    /// <summary>Generates unique identifier for BindableCache based on layout and slot</summary>
    static std::string GenerateUID(const D3::FinalizedLayout& layout, UINT slot);

    /// <summary>Generates unique identifier for BindableCache based on buffer data and slot</summary>
    static std::string GenerateUID(const D3::ConstantBufferData& buffer, UINT slot);

    /// <summary>Returns the unique identifier for this bindable instance</summary>
    std::string GetUID() const noexcept override;

private:
    /// <summary>Flag indicating whether the buffer data has been modified since last GPU upload</summary>
    bool isDirty = false;
    /// <summary>Cached copy of the constant buffer data on the CPU side</summary>
    D3::ConstantBufferData buffer;
};

/// <summary>
/// Non-caching version of dynamic constant buffer bindable that does not maintain
/// CPU-side data storage. This class is suitable for scenarios where constant buffer
/// data is provided externally and does not need to be cached or tracked for changes.
///
/// Key characteristics:
/// - No CPU-side data caching (lower memory usage)
/// - No dirty tracking (caller responsible for updates)
/// - Direct GPU uploads on every Update() call
/// - Cannot be shared via BindableCache (each instance is unique)
///
/// Use this class when:
/// - Constant buffer data is managed externally
/// - Memory usage is a concern
/// - Data updates are infrequent or controlled externally
/// - You don't need automatic dirty tracking
/// </summary>
class NoCacheDynamicPixelConstantBufferBindable : public DynamicPixelConstantBufferBindable
{
public:
    /// <summary>
    /// Creates a non-caching constant buffer bindable from a finalized layout.
    /// No initial data is uploaded to the GPU.
    /// </summary>
    /// <param name="gfx">Graphics context for buffer creation</param>
    /// <param name="layout">Finalized layout defining buffer structure</param>
    /// <param name="slot">Pixel shader slot number for binding</param>
    NoCacheDynamicPixelConstantBufferBindable(Graphics& gfx, const D3::FinalizedLayout& layout, UINT slot);

    /// <summary>
    /// Creates a non-caching constant buffer bindable from existing buffer data.
    /// The GPU buffer is initialized with the provided data, but no CPU copy is retained.
    /// </summary>
    /// <param name="gfx">Graphics context for buffer creation</param>
    /// <param name="buffer">Source data and layout for initial GPU upload</param>
    /// <param name="slot">Pixel shader slot number for binding</param>
    NoCacheDynamicPixelConstantBufferBindable(Graphics& gfx, const D3::ConstantBufferData& buffer, UINT slot);

    /// <summary>Returns the root layout element for this buffer</summary>
    const D3::LayoutElement& GetRootLayoutElement() const noexcept override;

private:
    /// <summary>
    /// Shared pointer to the root layout element. Keeps the layout alive
    /// for the lifetime of this bindable without storing buffer data.
    /// </summary>
    std::shared_ptr<D3::LayoutElement> pLayoutRoot;
};