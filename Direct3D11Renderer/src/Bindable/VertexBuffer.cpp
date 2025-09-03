#include "Bindable/VertexBuffer.h"

VertexBuffer::VertexBuffer(Graphics& gfx, std::string tag, const D3::VertexBuffer& vbuf) 
	: stride(UINT(vbuf.GetLayout().Size())), tag(tag)
	
{
	DEBUGMANAGER(gfx);
	D3D11_BUFFER_DESC bd = {};
	bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	bd.Usage = D3D11_USAGE_DEFAULT;
	bd.CPUAccessFlags = 0u;
	bd.MiscFlags = 0u;
	bd.ByteWidth = UINT(vbuf.SizeBytes());
	bd.StructureByteStride = stride;
	D3D11_SUBRESOURCE_DATA sd = {};
	sd.pSysMem = vbuf.GetData();
	GFX_THROW_INFO(GetDevice(gfx)->CreateBuffer(&bd, &sd, &pVertexBuffer));
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

std::shared_ptr<Bindable> VertexBuffer::Resolve(Graphics& gfx, const std::string& tag, const D3::VertexBuffer& vbuf)
{
	return BindableCache::Resolve<VertexBuffer>(gfx, tag, vbuf);
}

std::string VertexBuffer::GenerateUID_(const std::string& tag)
{
	return typeid(VertexBuffer).name() + std::string("#") + tag;
}
