#pragma once

#include "Bindable.h"
#include "Exceptions/BindableLookupException.h"
#include <unordered_map>
#include <stdexcept>

class BindableCache
{
public:
    BindableCache() = default;
    ~BindableCache() = default;
    BindableCache(const BindableCache&) = delete;
    BindableCache& operator=(const BindableCache&) = delete;

    template<typename T, typename... Args>
    static std::shared_ptr<T> Resolve(Graphics& gfx, Args&&... args)
    {
        static_assert(std::is_base_of<Bindable, T>::value, "Can only resolve classes derived from Bindable");
        
        // Generate the proper UID using the class's own method
        std::string uid = T::GenerateUID(std::forward<Args>(args)...);

        // Check if it already exists
        auto it = cache.find(uid);
        if (it != cache.end())
        {
            // Return existing instance if found
            return std::static_pointer_cast<T>(it->second);
        }

        // Create new bindable
        auto bindable = std::make_shared<T>(gfx, std::forward<Args>(args)...);
        cache[uid] = bindable;
        return bindable;
    }
private:
    static std::unordered_map<std::string, std::shared_ptr<Bindable>> cache;
};

// Define the static member outside the class
inline std::unordered_map<std::string, std::shared_ptr<Bindable>> BindableCache::cache;