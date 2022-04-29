
#include "index_buffer.h"

#include "../graphics_handler.h"

namespace hrzn::gfx
{
    IndexBuffer::IndexBuffer() :
		m_buffer(nullptr),
		m_indexCount(0)
	{
	}

	HRESULT IndexBuffer::initialize(DWORD* data, UINT indexCount)
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

		return GraphicsHandler::it().getDevice()->CreateBuffer(&indexBufferDesc, &indexBufferData, m_buffer.GetAddressOf());
	}

	ID3D11Buffer* IndexBuffer::getBuffer() const
	{
		return m_buffer.Get();
	}

	ID3D11Buffer* const* IndexBuffer::getAddressOfBuffer() const
	{
		return m_buffer.GetAddressOf();
	}

	UINT IndexBuffer::getIndexCount() const
	{
		return m_indexCount;
	}
}
