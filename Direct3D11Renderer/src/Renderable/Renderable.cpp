#include "Renderable/Renderable.h"
#include "Exceptions/GraphicsExceptions.h"
#include <cassert>
#include <typeinfo>

void Renderable::Render(Graphics& gfx) const noexcept(!_DEBUG)
{
    // Bind unique bindables
    for (auto& b : bindables)
    {
        b->Bind(gfx);
    }

    // Bind shared bindables
    for (auto& b : sharedBindables)
    {
        b->Bind(gfx);
    }

    gfx.DrawIndexed(pIndexBuffer->GetCount());
}

void Renderable::AddUniqueBindable(std::unique_ptr<Bindable> bindable) noexcept(!_DEBUG)
{
    // Check if this is an IndexBuffer
    if (auto* indexBuffer = dynamic_cast<IndexBuffer*>(bindable.get()))
    {
        // Handle index buffer logic
        assert("Attempting to add index buffer a second time" && pIndexBuffer == nullptr);
        pIndexBuffer = indexBuffer;
        bindables.push_back(std::move(bindable));
    }
    else
    {
        // Handle regular bindable
        bindables.push_back(std::move(bindable));
    }
}
