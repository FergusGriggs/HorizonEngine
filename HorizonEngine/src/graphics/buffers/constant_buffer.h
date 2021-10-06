
//Stores an ID3D11Buffer for a constant buffer of any type and its meta-data, automatically rounds the byte width to the 16 byte alignment

#pragma once

#include <d3d11.h>
#include <wrl/client.h>

#include "constant_buffer_types.h"
#include "../../utils/error_logger.h"

namespace hrzn::gfx
{
	template<class T>
	class ConstantBuffer
	{
	private:
		ConstantBuffer(const ConstantBuffer<T>& rhs);

	public:
		ConstantBuffer() :
			m_buffer(nullptr),
			m_deviceContext(nullptr)

		{
		}

		ID3D11Buffer* Get() const
		{
			return m_buffer.Get();
		}

		ID3D11Buffer* const* GetAddressOf() const
		{
			return m_buffer.GetAddressOf();
		}

		HRESULT Initialize(ID3D11Device* device, ID3D11DeviceContext* deviceContext)
		{
			if (m_buffer.Get() != nullptr)
			{
				m_buffer.Reset();
			}

			m_deviceContext = deviceContext;

			D3D11_BUFFER_DESC constantBufferDesc;
			ZeroMemory(&constantBufferDesc, sizeof(constantBufferDesc));

			constantBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
			constantBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
			constantBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
			constantBufferDesc.MiscFlags = 0;
			constantBufferDesc.ByteWidth = static_cast<UINT>(sizeof(T) + (16 - (sizeof(T) % 16)));
			constantBufferDesc.StructureByteStride = 0;

			return device->CreateBuffer(&constantBufferDesc, 0, m_buffer.GetAddressOf());
		}

		bool MapToGPU()
		{
			D3D11_MAPPED_SUBRESOURCE mappedResource;
			HRESULT hr = m_deviceContext->Map(m_buffer.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);

			if (FAILED(hr))
			{
				utils::ErrorLogger::log(hr, "Failed to map constant buffer.");
				return false;
			}

			CopyMemory(mappedResource.pData, &m_data, sizeof(T));
			m_deviceContext->Unmap(m_buffer.Get(), 0);

			return true;
		}

	public:
		T m_data;

	private:
		Microsoft::WRL::ComPtr<ID3D11Buffer> m_buffer;
		ID3D11DeviceContext* m_deviceContext;

	};
}
