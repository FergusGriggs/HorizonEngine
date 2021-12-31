#pragma once

#include <d3d11.h>
#include <wrl/client.h>

namespace hrzn::gfx
{
	struct IRenderTexture
	{
	protected:
		IRenderTexture();

		void initialise(DXGI_FORMAT format, UINT width, UINT height, bool isDepthTex);
		void release();

	public:
		bool canDirectCopyTo(IRenderTexture* renderTexture);

	public:
		DXGI_FORMAT     m_format;
		UINT            m_width;
		UINT            m_height;
		CD3D11_VIEWPORT m_viewport;

		Microsoft::WRL::ComPtr<ID3D11Texture2D>          m_texture2D = nullptr;
		Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> m_shaderResourceView = nullptr;
	};

	struct RenderTexture : public IRenderTexture
	{
		RenderTexture();

		void initialise(DXGI_FORMAT format, UINT width, UINT height);
		void release();

		void copyTo(RenderTexture* destination);
		void setAsRenderTargetAndDrawQuad(bool clearRenderTarget = true);

		Microsoft::WRL::ComPtr<ID3D11RenderTargetView> m_renderTargetView = nullptr;
	};

	struct DepthTexture : public IRenderTexture
	{
		DepthTexture();

		void initialise(DXGI_FORMAT format, UINT width, UINT height);
		void release();

		Microsoft::WRL::ComPtr<ID3D11DepthStencilView> m_depthStencilView = nullptr;
	};
}
