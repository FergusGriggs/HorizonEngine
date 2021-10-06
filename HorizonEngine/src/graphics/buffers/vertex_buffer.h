
//Stores an ID3D11Buffer for some vertices and their meta-data, automatically sets up buffer description and buffer subresource data

#pragma once

#include <memory>

#include <d3d11.h>
#include <wrl/client.h>

namespace hrzn::gfx
{
	template<class T>
	class VertexBuffer
	{
	public:
		VertexBuffer() :
			m_buffer(nullptr),
			m_stride(sizeof(T)),
			m_vertexCount(0)
		{
		}

		VertexBuffer(const VertexBuffer<T>& rhs)
		{
			m_buffer = rhs.m_buffer;
			m_vertexCount = rhs.m_vertexCount;
			m_stride = rhs.m_stride;
		}

		VertexBuffer<T>& operator=(const VertexBuffer<T>& a)
		{
			m_buffer = a.m_buffer;
			m_vertexCount = a.m_vertexCount;
			m_stride = a.m_stride;
			return *this;
		}

		ID3D11Buffer* Get() const
		{
			return m_buffer.Get();
		}

		ID3D11Buffer* const* GetAddressOf() const
		{
			return m_buffer.GetAddressOf();
		}

		UINT VertexCount() const
		{
			return m_vertexCount;
		}

		const UINT Stride() const
		{
			return m_stride;
		}

		const UINT* StridePtr() const
		{
			return &m_stride;
		}

		HRESULT Initialize(ID3D11Device* device, T* data, UINT vertexCount)
		{
			if (m_buffer.Get() != nullptr)
			{
				m_buffer.Reset();
			}

			m_vertexCount = vertexCount;

			D3D11_BUFFER_DESC vertexBufferDesc;
			ZeroMemory(&vertexBufferDesc, sizeof(vertexBufferDesc));

			vertexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
			vertexBufferDesc.ByteWidth = m_stride * m_vertexCount;
			vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
			vertexBufferDesc.CPUAccessFlags = 0;
			vertexBufferDesc.MiscFlags = 0;

			D3D11_SUBRESOURCE_DATA vertexBufferData;
			ZeroMemory(&vertexBufferData, sizeof(vertexBufferData));

			vertexBufferData.pSysMem = data;

			return device->CreateBuffer(&vertexBufferDesc, &vertexBufferData, m_buffer.GetAddressOf());
		}

	private:
		Microsoft::WRL::ComPtr<ID3D11Buffer> m_buffer;
		UINT                                 m_stride;
		UINT                                 m_vertexCount;
	};
}