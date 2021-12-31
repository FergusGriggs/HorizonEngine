#pragma once

#include <d3d11.h>

namespace hrzn::gfx
{
	struct RenderPassConfig
	{
		CD3D11_VIEWPORT          m_viewport;

		ID3D11RenderTargetView** m_renderTargetViews;
		// MAX OF 8
		int                      m_numRenderTargetViews;

		ID3D11DepthStencilView*  m_depthStencilView;

		ID3D11DepthStencilState* m_depthStencilState;
		ID3D11RasterizerState*   m_rasterizerState;
		ID3D11BlendState*        m_blendState;

		int                      m_highestLOD;
	};
}

