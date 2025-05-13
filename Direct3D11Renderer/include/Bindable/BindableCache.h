#pragma once

#include "Bindable.h"
#include "BindableKey.h"
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

    // Create a new bindable and store it in the cache
    template<typename T, typename... Args>
    static std::shared_ptr<T> Create(Graphics& gfx, const std::string& id, Args&&... args)
    {
        auto key = BindableKey(typeid(T), id);

        // Check if it already exists
        auto it = cache.find(key);
        if (it != cache.end())
        {
            // Return existing instance if found
            return std::static_pointer_cast<T>(it->second);
        }

        // Create new bindable
        auto bindable = std::make_shared<T>(gfx, std::forward<Args>(args)...);
        cache[key] = bindable;
        return bindable;
    }

    // Find an existing bindable without creating it
    template<typename T>
    static std::shared_ptr<T> Find(const std::string& id)
    {
        auto key = BindableKey(typeid(T), id);
        auto it = cache.find(key);
        if (it != cache.end())
        {
            return std::static_pointer_cast<T>(it->second);
        }

        // Throw exception if not found
        throw BINDABLE_LOOKUP_EXCEPT(typeid(T).name(), id);
    }

    // Find an existing bindable with a default value if not found
    template<typename T>
    static std::shared_ptr<T> FindOrDefault(const std::string& id, std::shared_ptr<T> defaultValue = nullptr)
    {
        auto key = BindableKey(typeid(T), id);
        auto it = cache.find(key);
        if (it != cache.end())
        {
            return std::static_pointer_cast<T>(it->second);
        }

        // Return default value if not found
        return defaultValue;
    }

    // Check if a bindable exists
    template<typename T>
    static bool Exists(const std::string& id)
    {
        auto key = BindableKey(typeid(T), id);
        return cache.find(key) != cache.end();
    }

    // Remove a bindable from the cache
    template<typename T>
    static bool Remove(const std::string& id)
    {
        auto key = BindableKey(typeid(T), id);
        return cache.erase(key) > 0;
    }

    // Clear the entire cache
    static void Clear()
    {
        cache.clear();
    }

private:
    static std::unordered_map<BindableKey, std::shared_ptr<Bindable>, BindableKey::Hash> cache;
};

// Define the static member outside the class
inline std::unordered_map<BindableKey, std::shared_ptr<Bindable>, BindableKey::Hash> BindableCache::cache;