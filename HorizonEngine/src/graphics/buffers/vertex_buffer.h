
//Stores an ID3D11Buffer for some vertices and their meta-data, automatically sets up buffer description and buffer subresource data

#pragma once

#include <memory>

#include <d3d11.h>
#include <wrl/client.h>

namespace hrzn::gfx
{
	class VertexBuffer
	{
	public:
		VertexBuffer();
		VertexBuffer(const VertexBuffer& rhs);
		VertexBuffer& operator=(const VertexBuffer& a);

		HRESULT initialise(const void* data, UINT stride, UINT vertexCount);

		ID3D11Buffer*        getBuffer() const;
		ID3D11Buffer* const* getAddressOfBuffer() const;

		const UINT  getStride() const;
		const UINT* getStridePtr() const;

		UINT getVertexCount() const;

	private:
		Microsoft::WRL::ComPtr<ID3D11Buffer> m_buffer;
		UINT                                 m_stride;
		UINT                                 m_vertexCount;
	};
}
