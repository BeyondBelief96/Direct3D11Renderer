#pragma once
#include "Graphics.h"
#include "Bindable/Bindable.h"
#include "Bindable/IndexBuffer.h"
#include "Bindable/BindableCache.h"
#include <memory>
#include <vector>
#include <DirectXMath.h>

class Renderable
{
public:
    Renderable() = default;
    Renderable(const Renderable&) = delete;
    virtual ~Renderable() = default;

    void Render(Graphics& gfx) const noexcept(!_DEBUG);

    void AddUniqueBindable(std::unique_ptr<Bindable> bindable) noexcept(!_DEBUG);

    template<typename T, typename... Args>
    std::shared_ptr<T> AddSharedBindable(Graphics& gfx, const std::string& id, Args&&... args)
    {
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
    std::shared_ptr<T> GetSharedBindable(const std::string& id)
    {
        return BindableCache::Find<T>(id);
    }

    template<typename T>
    bool HasBindable(const std::string& id)
    {
        return BindableCache::Exists<T>(id);
    }

    template<typename T>
    std::shared_ptr<T> GetBindableOrDefault(const std::string& id, std::shared_ptr<T> defaultValue = nullptr)
    {
        return BindableCache::FindOrDefault<T>(id, defaultValue);
    }

    virtual DirectX::XMMATRIX GetTransformXM() const noexcept = 0;
    virtual void Update(float dt) noexcept = 0;

private:
    const IndexBuffer* pIndexBuffer = nullptr;
    std::vector<std::unique_ptr<Bindable>> bindables;
    std::vector<std::shared_ptr<Bindable>> sharedBindables;
};