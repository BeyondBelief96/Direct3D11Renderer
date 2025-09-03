#include "Bindable/VertexBuffer.h"

VertexBuffer::VertexBuffer(Graphics& gfx, std::string tag, const D3::VertexBuffer& vbuf) 
	: stride(UINT(vbuf.GetLayout().Size())), tag(std::move(tag))
	
{
	DEBUGMANAGER(gfx);
	D3D11_BUFFER_DESC desc = {};
	desc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	desc.Usage = D3D11_USAGE_DEFAULT;
	desc.CPUAccessFlags = 0;
	desc.MiscFlags = 0;
	desc.ByteWidth = (UINT)vbuf.SizeBytes();
	desc.StructureByteStride = stride;

	D3D11_SUBRESOURCE_DATA init = {};
	init.pSysMem = vbuf.GetData();

	GFX_THROW_INFO(GetDevice(gfx)->CreateBuffer(&desc, &init, &pVertexBuffer));
}

VertexBuffer::VertexBuffer(Graphics& gfx, const D3::VertexBuffer& vbuf) noexcept(!_DEBUG)
	: VertexBuffer(gfx, "?", vbuf)
{
}

void VertexBuffer::Bind(Graphics& gfx) noexcept
{
	const UINT offset = 0u;
	GetContext(gfx)->IASetVertexBuffers(0u, 1u, pVertexBuffer.GetAddressOf(), &stride, &offset);
}

std::string VertexBuffer::GetUID() const noexcept
{
	return GenerateUID(tag);
}

std::shared_ptr<Bindable> VertexBuffer::Resolve(Graphics& gfx, const D3::VertexBuffer& vbuf, const std::string& tag)
{
	return BindableCache::Resolve<VertexBuffer>(gfx, tag, vbuf);
}

std::string VertexBuffer::GenerateUID_(const std::string& tag)
{
	return typeid(VertexBuffer).name() + std::string("#") + tag;
}
