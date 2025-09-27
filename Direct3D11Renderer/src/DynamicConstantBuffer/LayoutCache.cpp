#include "DynamicConstantBuffer/LayoutCache.h"

namespace D3
{
    /// <summary>
    /// Resolves a LayoutBuilder into a FinalizedLayout using caching for performance optimization.
    ///
    /// Algorithm:
    /// 1. Generate a signature string from the current builder state
    /// 2. Check if a layout with this signature already exists in the cache
    /// 3. If found: reset the builder and return the cached layout
    /// 4. If not found: extract and finalize the layout from the builder, cache it, then return it
    ///
    /// This approach ensures that identical layouts (same structure, member names, and types)
    /// are shared across multiple ConstantBufferData instances, reducing memory usage and
    /// improving performance.
    /// </summary>
    /// <param name="builder">LayoutBuilder to resolve (will be consumed/reset)</param>
    /// <returns>FinalizedLayout ready for use in creating ConstantBufferData</returns>
    FinalizedLayout LayoutCache::Resolve(LayoutBuilder&& builder)
    {
        // Generate signature before extracting to avoid modifying the builder
        auto signature = builder.GetSignature();
        auto& instance = GetInstance();

        // Check if we already have a layout with this signature
        auto it = instance.layoutCache.find(signature);
        if (it != instance.layoutCache.end())
        {
            // Cache hit: layout already exists, clear the builder and return cached layout
            builder.Reset();
            return FinalizedLayout(it->second);
        }

        // Cache miss: extract and finalize the new layout, then cache it
        auto layoutRoot = builder.ExtractRoot();  // This finalizes the layout and resets the builder
        auto result = instance.layoutCache.emplace(std::move(signature), layoutRoot);

        return FinalizedLayout(result.first->second);
    }

    /// <summary>
    /// Provides access to the singleton LayoutCache instance using the Meyers' singleton pattern.
    /// The static local variable ensures thread-safe initialization in C++11 and later,
    /// and automatic cleanup at program termination.
    ///
    /// This pattern is preferred over global variables because:
    /// - Initialization is deferred until first use
    /// - No issues with static initialization order
    /// - Automatic cleanup is guaranteed
    /// </summary>
    /// <returns>Reference to the singleton LayoutCache instance</returns>
    LayoutCache& LayoutCache::GetInstance()
    {
        static LayoutCache instance;
        return instance;
    }
}