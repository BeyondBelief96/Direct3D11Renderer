#include "Renderable/Renderable.h"
#include "Exceptions/GraphicsExceptions.h"
#include <cassert>
#include <typeinfo>

void Renderable::Render(Graphics& gfx) const noexcept(!_DEBUG)
{
    for (auto& b : bindables)
    {
        b->Bind(gfx);
    }

    gfx.DrawIndexed(pIndexBuffer->GetCount());
}

void Renderable::AddBindable(std::shared_ptr<Bindable> bindable)
{
    if (typeid(*bindable) == typeid(IndexBuffer))
    {
        assert("Binding multiple index buffers not allowed" && pIndexBuffer == nullptr);
        pIndexBuffer = &static_cast<IndexBuffer&>(*bindable);
    }

	bindables.push_back(std::move(bindable));
}
