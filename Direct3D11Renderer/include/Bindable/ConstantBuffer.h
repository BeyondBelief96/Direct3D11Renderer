#pragma once

#include "Bindable.h"
#include "Exceptions/GraphicsExceptions.h"
#include <wrl.h>

template<typename C>
class ConstantBuffer : public Bindable
{
public:
	ConstantBuffer(Graphics& gfx, const C& constBufferData)
	{
		INFOMAN(gfx);
		D3D11_BUFFER_DESC bufferDesc{};
		bufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
		bufferDesc.Usage = D3D11_USAGE_DYNAMIC;
		bufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
		bufferDesc.MiscFlags = 0u;
		bufferDesc.ByteWidth = sizeof(C);
		bufferDesc.StructureByteStride = 0u;
		D3D11_SUBRESOURCE_DATA bufferData{};
		bufferData.pSysMem = &constBufferData;
		GFX_THROW_INFO(GetDevice(gfx)->CreateBuffer(&bufferDesc, &bufferData, pConstantBuffer.GetAddressOf()));
	}
	ConstantBuffer(Graphics& gfx)
	{
		INFOMAN(gfx);
		D3D11_BUFFER_DESC bufferDesc{};
		bufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
		bufferDesc.Usage = D3D11_USAGE_DYNAMIC;
		bufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
		bufferDesc.MiscFlags = 0u;
		bufferDesc.ByteWidth = sizeof(C);
		bufferDesc.StructureByteStride = 0u;
		GFX_THROW_INFO(GetDevice(gfx)->CreateBuffer(&bufferDesc, nullptr, pConstantBuffer.GetAddressOf()));
	}
	void Update(Graphics& gfx, const C& constBufferData)
	{
		INFOMAN(gfx);
		D3D11_MAPPED_SUBRESOURCE mappedResource;
		GFX_THROW_INFO(GetContext(gfx)->Map(pConstantBuffer.Get(), 0u, D3D11_MAP_WRITE_DISCARD, 0u, &mappedResource));
		memcpy(mappedResource.pData, &constBufferData, sizeof(C));
		GetContext(gfx)->Unmap(pConstantBuffer.Get(), 0u);
	}
protected:
	Microsoft::WRL::ComPtr<ID3D11Buffer> pConstantBuffer;
};

template<typename C>
class VertexConstantBuffer : public ConstantBuffer<C>
{
	using ConstantBuffer<C>::pConstantBuffer;
public:
	using ConstantBuffer<C>::ConstantBuffer;
	void Bind(Graphics& gfx) noexcept override
	{
		Bindable::GetContext(gfx)->VSSetConstantBuffers(0u, 1u, pConstantBuffer.GetAddressOf());
	}
};

template<typename C>
class PixelConstantBuffer : public ConstantBuffer<C>
{
	using ConstantBuffer<C>::pConstantBuffer;
	using Bindable::GetContext;
public:
	using ConstantBuffer<C>::ConstantBuffer;
	void Bind(Graphics& gfx) noexcept override
	{
		GetContext(gfx)->PSSetConstantBuffers(0u, 1u, pConstantBuffer.GetAddressOf());
	}
};
