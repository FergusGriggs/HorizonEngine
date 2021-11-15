#pragma once

#include <d3d11.h>

struct RenderPassConfig
{
	CD3D11_VIEWPORT         m_viewport;

	ID3D11RenderTargetView* m_renderTargetView;
	ID3D11DepthStencilView* m_depthStencilView;
	ID3D11RasterizerState*  m_rasterizerState;
	ID3D11BlendState*       m_blendState;

	int                     m_highestLOD;
};