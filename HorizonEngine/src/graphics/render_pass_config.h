#pragma once

#include <d3d11.h>

struct RenderPassConfig
{
	CD3D11_VIEWPORT          m_viewport;

	ID3D11RenderTargetView** m_renderTargetViews;
	int                      m_numRenderTargetViews;

	ID3D11DepthStencilView*  m_depthStencilView;

	ID3D11DepthStencilState* m_depthStencilState;
	ID3D11RasterizerState*   m_rasterizerState;
	ID3D11BlendState*        m_blendState;

	XMMATRIX                 m_viewMatrix;
	XMMATRIX                 m_projectionMatrix;

	int                      m_highestLOD;
};