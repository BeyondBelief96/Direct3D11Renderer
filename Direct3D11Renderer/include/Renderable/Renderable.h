#pragma once
#include "Core/Graphics.h"
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
    void AddBindable(std::shared_ptr<Bindable> bindable);

    virtual DirectX::XMMATRIX GetTransformXM() const noexcept = 0;

private:
    const IndexBuffer* pIndexBuffer = nullptr;
    std::vector<std::shared_ptr<Bindable>> bindables;
};