#pragma once
#include "Core/Graphics.h"
#include "Bindable/Bindable.h"
#include "Bindable/IndexBuffer.h"
#include "Bindable/BindableCache.h"
#include "RenderPass/Technique.h"
#include <memory>
#include <vector>
#include <DirectXMath.h>
#include <typeinfo>

class IndexBuffer;
class VertexBuffer;
class Topology;
class InputLayout;
class TechniqueProbe;

class Renderable
{
public:
    Renderable() = default;
    Renderable(const Renderable&) = delete;
    virtual ~Renderable() = default;
	void AddTechnique(Technique technique) noexcept;
    void Submit(class FrameManager& frameManager) const noexcept;
    void Accept(TechniqueProbe& probe);
    virtual DirectX::XMMATRIX GetTransformXM() const noexcept = 0;
    UINT GetIndexCount() const noexcept;
    void Bind(Graphics& gfx) const noexcept;
protected:

    std::shared_ptr<IndexBuffer> pIndices;
    std::shared_ptr<VertexBuffer> pVertices;
    std::shared_ptr<Topology> pTopology;
    std::vector<Technique> techniques;
};