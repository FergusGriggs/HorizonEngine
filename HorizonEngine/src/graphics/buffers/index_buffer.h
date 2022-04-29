
#pragma once

#include <d3d11.h>
#include <wrl/client.h>

namespace hrzn::gfx
{
	class IndexBuffer
	{
	public:
		IndexBuffer();

		HRESULT              initialize(DWORD* data, UINT indexCount);

		ID3D11Buffer*        getBuffer() const;
		ID3D11Buffer* const* getAddressOfBuffer() const;

		UINT                 getIndexCount() const;

	private:
		Microsoft::WRL::ComPtr<ID3D11Buffer> m_buffer;
		UINT                                 m_indexCount;
	};
}
