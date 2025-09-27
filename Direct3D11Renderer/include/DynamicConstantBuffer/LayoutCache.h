#pragma once

#include "DynamicConstantBuffer.h"
#include <unordered_map>
#include <string>
#include <memory>

namespace D3
{
    /// <summary>
    /// Singleton cache for layout definitions that prevents duplicate layout creation
    /// and enables efficient sharing of identical layouts across multiple ConstantBufferData instances.
    ///
    /// The cache uses layout signatures as keys to identify identical layouts. When a layout
    /// is requested, the cache either returns an existing finalized layout or creates a new one
    /// from the provided builder. This optimization is crucial for performance when many objects
    /// use the same constant buffer layout (e.g., multiple meshes with the same material type).
    ///
    /// Thread Safety: This class is not thread-safe. It should only be used from the main
    /// rendering thread or proper synchronization must be implemented.
    /// </summary>
    class LayoutCache
    {
    public:
        /// <summary>
        /// Resolves a LayoutBuilder into a FinalizedLayout, either by returning a cached
        /// layout with the same signature or by finalizing the builder and caching the result.
        ///
        /// The builder is consumed in the process - it will be reset to empty state regardless
        /// of whether a cached layout was found or a new one was created.
        /// </summary>
        /// <param name="builder">LayoutBuilder to resolve (passed by rvalue reference, will be consumed)</param>
        /// <returns>FinalizedLayout that can be used to create ConstantBufferData instances</returns>
        static FinalizedLayout Resolve(LayoutBuilder&& builder);

    private:
        /// <summary>
        /// Gets the singleton instance of the LayoutCache. Uses static local variable
        /// pattern to ensure proper initialization and cleanup.
        /// </summary>
        /// <returns>Reference to the singleton LayoutCache instance</returns>
        static LayoutCache& GetInstance();

        /// <summary>
        /// Hash map that stores finalized layouts keyed by their signature strings.
        /// The signature uniquely identifies the structure of a layout, allowing
        /// identical layouts to be shared even if they were created independently.
        /// </summary>
        std::unordered_map<std::string, std::shared_ptr<LayoutElement>> layoutCache;
    };
}