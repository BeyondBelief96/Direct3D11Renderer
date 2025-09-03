#include "Bindable/IndexBuffer.h"
#include "Bindable/BindableCache.h"

IndexBuffer::IndexBuffer(Graphics& gfx, const std::vector<unsigned short>& indices)
	: IndexBuffer(gfx, "?", indices)
{
}

IndexBuffer::IndexBuffer(Graphics& gfx, std::string tag, const std::vector<unsigned short>& indices)
	: count((UINT)indices.size()), tag(tag)
{
	DEBUGMANAGER(gfx);

	D3D11_BUFFER_DESC indexBufferDesc{};
	indexBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	indexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	indexBufferDesc.CPUAccessFlags = 0u;
	indexBufferDesc.MiscFlags = 0u;
	indexBufferDesc.ByteWidth = (UINT)(count * sizeof(unsigned short));
	indexBufferDesc.StructureByteStride = sizeof(unsigned short);
	D3D11_SUBRESOURCE_DATA indexBufferData{};
	indexBufferData.pSysMem = indices.data();
	GFX_THROW_INFO(GetDevice(gfx)->CreateBuffer(&indexBufferDesc, &indexBufferData, &pIndexBuffer));
}

void IndexBuffer::Bind(Graphics& gfx) noexcept
{
	GetContext(gfx)->IASetIndexBuffer(pIndexBuffer.Get(), DXGI_FORMAT_R16_UINT, 0u);
}

UINT IndexBuffer::GetCount() const noexcept
{
	return count;
}

std::string IndexBuffer::GetUID() const noexcept
{
	return GenerateUID_(tag);
}

std::shared_ptr<IndexBuffer> IndexBuffer::Resolve(Graphics& gfx, const std::string tag, const std::vector<unsigned short>& indices)
{
	return BindableCache::Resolve<IndexBuffer>(gfx, tag, indices);
}

std::string IndexBuffer::GenerateUID_(const std::string& tag)
{
	return typeid(IndexBuffer).name() + std::string("#") + tag;
}
