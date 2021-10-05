//ConstantBuffer.h
//Stores an ID3D11Buffer for a constant buffer of any type and its meta-data, automatically rounds the byte width to the 16 byte alignment

#pragma once

#ifndef ConstantBuffer_h__
#define ConstantBuffer_h__

#include <d3d11.h>
#include <wrl/client.h>

#include "ConstantBufferTypes.h"
#include "../utility/ErrorLogger.h"

template<class T>
class ConstantBuffer
{
private:
	ConstantBuffer(const ConstantBuffer<T>& rhs);

private:
	Microsoft::WRL::ComPtr<ID3D11Buffer> buffer;
	ID3D11DeviceContext* deviceContext = nullptr;

public:
	ConstantBuffer() {}
	T data;

	ID3D11Buffer* Get() const
	{
		return buffer.Get();
	}

	ID3D11Buffer* const* GetAddressOf() const
	{
		return buffer.GetAddressOf();
	}

	HRESULT Initialize(ID3D11Device* device, ID3D11DeviceContext* deviceContext)
	{
		if (this->buffer.Get() != nullptr) {
			this->buffer.Reset();
		}

		this->deviceContext = deviceContext;

		D3D11_BUFFER_DESC constantBufferDesc;
		ZeroMemory(&constantBufferDesc, sizeof(constantBufferDesc));

		constantBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
		constantBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
		constantBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
		constantBufferDesc.MiscFlags = 0;
		constantBufferDesc.ByteWidth = static_cast<UINT>(sizeof(T) + (16 - (sizeof(T) % 16)));
		constantBufferDesc.StructureByteStride = 0;

		return device->CreateBuffer(&constantBufferDesc, 0, this->buffer.GetAddressOf());
	}

	bool MapToGPU() {
		D3D11_MAPPED_SUBRESOURCE mappedResource;
		HRESULT hr = this->deviceContext->Map(buffer.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);

		if (FAILED(hr)) {
			ErrorLogger::Log(hr, "Failed to map constant buffer.");
			return false;
		}

		CopyMemory(mappedResource.pData, &data, sizeof(T));
		this->deviceContext->Unmap(buffer.Get(), 0);

		return true;
	}

};

#endif