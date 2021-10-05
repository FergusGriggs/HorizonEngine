//IndexBuffer.h
//Stores an ID3D11Buffer for some indices and their meta-data, automatically creates buffer description and buffer subresource data

#pragma once

#ifndef IndexBuffer_h__
#define IndexBuffer_h__

#include <d3d11.h>
#include <wrl/client.h>

class IndexBuffer
{
private:
	IndexBuffer(const IndexBuffer& rhs);

private:
	Microsoft::WRL::ComPtr<ID3D11Buffer> buffer;
	UINT indexCount = 0;

public:
	IndexBuffer() {}

	ID3D11Buffer* Get() const
	{
		return buffer.Get();
	}

	ID3D11Buffer* const* GetAddressOf() const
	{
		return buffer.GetAddressOf();
	}

	UINT IndexCount() const {
		return this->indexCount;
	}

	HRESULT Initialize(ID3D11Device* device, DWORD * data, UINT indexCount)
	{
		if (this->buffer.Get() != nullptr) {
			this->buffer.Reset();
		}

		this->indexCount = indexCount;

		D3D11_BUFFER_DESC indexBufferDesc;
		ZeroMemory(&indexBufferDesc, sizeof(indexBufferDesc));

		indexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
		indexBufferDesc.ByteWidth = sizeof(DWORD) * indexCount;
		indexBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
		indexBufferDesc.CPUAccessFlags = 0;
		indexBufferDesc.MiscFlags = 0;

		D3D11_SUBRESOURCE_DATA indexBufferData;
		ZeroMemory(&indexBufferData, sizeof(indexBufferData));

		indexBufferData.pSysMem = data;

		return device->CreateBuffer(&indexBufferDesc, &indexBufferData, this->buffer.GetAddressOf());
	}
};

#endif