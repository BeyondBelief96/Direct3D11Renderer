#pragma once

#include "Graphics.h"
#include "Bindable/Bindable.h"
#include "Bindable/IndexBuffer.h"
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
	void AddBindable(std::unique_ptr<Bindable> bindable) noexcept(!_DEBUG);

	virtual DirectX::XMMATRIX GetTransformXM() const noexcept = 0;
	virtual void Update(float dt) noexcept = 0;
private:
	const IndexBuffer* pIndexBuffer = nullptr;
	std::vector<std::unique_ptr<Bindable>> bindables;
};