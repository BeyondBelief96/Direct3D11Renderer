#pragma once
#include "Graphics/Graphics.h"
#include "Bindable/Bindable.h"
#include "Bindable/IndexBuffer.h"
#include "Bindable/BindableCache.h"
#include <memory>
#include <vector>
#include <DirectXMath.h>
#include <typeinfo>

class Renderable
{
public:
    Renderable() = default;
    Renderable(const Renderable&) = delete;
    virtual ~Renderable() = default;

    void Render(Graphics& gfx) const noexcept(!_DEBUG);
    void AddUniqueBindable(std::unique_ptr<Bindable> bindable) noexcept(!_DEBUG);

    // Safer template method to get unique bindable with type checking
    template<typename T>
    T* QueryUniqueBindable() noexcept
    {
        static_assert(std::is_base_of<Bindable, T>::value, "T must inherit from Bindable");

        for (auto& bindable : bindables)
        {
            if (auto* ptr = dynamic_cast<T*>(bindable.get()))
            {
                return ptr;
            }
        }
        return nullptr;
    }

    // Check if a bindable of specific type exists
    template<typename T>
    bool HasUniqueBindable() const noexcept
    {
        static_assert(std::is_base_of<Bindable, T>::value, "T must inherit from Bindable");

        for (const auto& bindable : bindables)
        {
            if (dynamic_cast<T*>(bindable.get()) != nullptr)
            {
                return true;
            }
        }
        return false;
    }

    // Add a bindable and return a pointer to it for further configuration
    template<typename T, typename... Args>
    T* AddUniqueBindableAndGet(Args&&... args) noexcept(!_DEBUG)
    {
        static_assert(std::is_base_of<Bindable, T>::value, "T must inherit from Bindable");

        auto bindable = std::make_unique<T>(std::forward<Args>(args)...);
        T* raw_ptr = bindable.get();

        // Handle index buffer specially
        if (auto* indexBuffer = dynamic_cast<IndexBuffer*>(raw_ptr))
        {
            pIndexBuffer = indexBuffer;
        }

        bindables.push_back(std::move(bindable));
        return raw_ptr;
    }

    // Update a bindable if it exists, create it if it doesn't
    template<typename T, typename... Args>
    T* UpdateOrCreateUniqueBindable(Args&&... args) noexcept(!_DEBUG)
    {
        static_assert(std::is_base_of<Bindable, T>::value, "T must inherit from Bindable");

        // First try to find an existing bindable of type T
        for (auto& bindable : bindables)
        {
            if (auto* ptr = dynamic_cast<T*>(bindable.get()))
            {
                return ptr; // Return existing bindable
            }
        }

        // If not found, create a new one
        return AddUniqueBindableAndGet<T>(std::forward<Args>(args)...);
    }

    // Shared bindable management
    template<typename T, typename... Args>
    std::shared_ptr<T> AddSharedBindable(Graphics& gfx, const std::string& id, Args&&... args)
    {
        static_assert(std::is_base_of<Bindable, T>::value, "T must inherit from Bindable");

        auto shared = BindableCache::Create<T>(gfx, id, std::forward<Args>(args)...);

        // Handle index buffer specially
        if (auto* indexBuffer = dynamic_cast<IndexBuffer*>(shared.get()))
        {
            pIndexBuffer = indexBuffer;
        }

        sharedBindables.push_back(shared);
        return shared;
    }

    template<typename T>
    std::shared_ptr<T> QuerySharedBindable() noexcept
    {
        static_assert(std::is_base_of<Bindable, T>::value, "T must inherit from Bindable");

        for (auto& bindable : sharedBindables)
        {
            if (auto ptr = std::dynamic_pointer_cast<T>(bindable))
            {
                return ptr;
            }
        }
        return nullptr;
    }

    virtual DirectX::XMMATRIX GetTransformXM() const noexcept = 0;
    virtual void Update(float dt) noexcept {};

private:
    const IndexBuffer* pIndexBuffer = nullptr;
    std::vector<std::unique_ptr<Bindable>> bindables;
    std::vector<std::shared_ptr<Bindable>> sharedBindables;
};