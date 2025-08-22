#pragma once

#include "Bindable.h"
#include "Exceptions/GraphicsExceptions.h"
#include <vector>
#include <wrl.h>

class VertexBuffer : public Bindable
{
public:
	template<class V>
	VertexBuffer(Graphics& gfx, const std::vector<V> vertices)
		: stride(sizeof(V))
	{
		DEBUGMANAGER(gfx);
		D3D11_BUFFER_DESC vertexBufferDesc{};
		vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
		vertexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
		vertexBufferDesc.CPUAccessFlags = 0u;
		vertexBufferDesc.MiscFlags = 0u;
		vertexBufferDesc.ByteWidth = (UINT)(sizeof(V) * vertices.size());
		vertexBufferDesc.StructureByteStride = sizeof(V);
		D3D11_SUBRESOURCE_DATA vertexBufferData{};
		vertexBufferData.pSysMem = vertices.data();
		GFX_THROW_INFO(GetDevice(gfx)->CreateBuffer(&vertexBufferDesc, &vertexBufferData, &pVertexBuffer));
	}

	void Bind(Graphics& gfx) noexcept override;
protected:
	UINT stride;
	Microsoft::WRL::ComPtr<ID3D11Buffer> pVertexBuffer;
};
