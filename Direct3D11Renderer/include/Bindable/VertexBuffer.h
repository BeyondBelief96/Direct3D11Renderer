#pragma once

#include "BindableCommon.h"
#include "Exceptions/GraphicsExceptions.h"
#include "Geometry/Vertex.h"
#include <vector>
#include <wrl.h>

class VertexBuffer : public Bindable
{
public:
	VertexBuffer(Graphics& gfx, std::string tag, const D3::VertexBuffer& vbuf);
	VertexBuffer(Graphics& gfx, const D3::VertexBuffer& vbuf) noexcept(!_DEBUG);
	void Bind(Graphics& gfx) noexcept override;
	std::string GetUID() const noexcept override;

	static std::shared_ptr<Bindable> Resolve(Graphics& gfx, const D3::VertexBuffer& vbuf, const std::string& tag);

	template<typename ... Ignore>
	static std::string GenerateUID(const std::string& tag, Ignore&&... ignore)
	{
		return GenerateUID_(tag);
	}
protected:
	UINT stride;
	std::string tag;
	Microsoft::WRL::ComPtr<ID3D11Buffer> pVertexBuffer;
private:
	static std::string GenerateUID_(const std::string& tag);
};
