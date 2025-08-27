#include "Bindable/VertexBuffer.h"

VertexBuffer::VertexBuffer(Graphics& gfx, const D3::VertexBuffer& vbuf) noexcept(!_DEBUG)
	: stride((UINT)vbuf.GetLayout().Size())
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

void VertexBuffer::Bind(Graphics& gfx) noexcept
{
	const UINT offset = 0u;
	GetContext(gfx)->IASetVertexBuffers(0u, 1u, pVertexBuffer.GetAddressOf(), &stride, &offset);
}
