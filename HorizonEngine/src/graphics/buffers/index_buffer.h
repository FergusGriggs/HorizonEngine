
//Stores an ID3D11Buffer for some indices and their meta-data, automatically creates buffer description and buffer subresource data

#pragma once

#include <d3d11.h>
#include <wrl/client.h>

namespace hrzn::gfx
{
	class IndexBuffer
	{
	public:
		IndexBuffer() :
			m_buffer(nullptr),
			m_indexCount(0)
		{
		};

		ID3D11Buffer* get() const
		{
			return m_buffer.Get();
		}

		ID3D11Buffer* const* getAddressOf() const
		{
			return m_buffer.GetAddressOf();
		}

		UINT indexCount() const
		{
			return m_indexCount;
		}

		HRESULT initialize(ID3D11Device* device, DWORD* data, UINT indexCount)
		{
			if (m_buffer.Get() != nullptr)
			{
				m_buffer.Reset();
			}

			m_indexCount = indexCount;

			D3D11_BUFFER_DESC indexBufferDesc;
			ZeroMemory(&indexBufferDesc, sizeof(indexBufferDesc));

			indexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
			indexBufferDesc.ByteWidth = sizeof(DWORD) * m_indexCount;
			indexBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
			indexBufferDesc.CPUAccessFlags = 0;
			indexBufferDesc.MiscFlags = 0;

			D3D11_SUBRESOURCE_DATA indexBufferData;
			ZeroMemory(&indexBufferData, sizeof(indexBufferData));

			indexBufferData.pSysMem = data;

			return device->CreateBuffer(&indexBufferDesc, &indexBufferData, m_buffer.GetAddressOf());
		}

	private:
		Microsoft::WRL::ComPtr<ID3D11Buffer> m_buffer;
		UINT                                 m_indexCount;
	};
}
