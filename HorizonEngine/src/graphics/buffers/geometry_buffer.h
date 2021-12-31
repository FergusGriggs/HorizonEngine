#pragma once

#include "../data/render_textures.h"

namespace hrzn::gfx
{
	class GeometryBuffer
	{
	public:
		GeometryBuffer();
		~GeometryBuffer();

		void initialise(UINT texWidth, UINT texHeight);
		void release();

	public:
		// Albedo -> DXGI_FORMAT_R8G8B8A8_UNORM
		// RGBA -> Pixel colour
		RenderTexture m_albedo;

		// PositionRoughness -> DXGI_FORMAT_R32G32B32A32_FLOAT
		// RGB -> Pixel position | A -> Pixel roughness
		RenderTexture m_positionRoughness;

		// NormalAO -> DXGI_FORMAT_R32G32B32A32_FLOAT
		// RGB -> Pixel normal | A -> Pixel ambient occlusion
		RenderTexture m_normalAO;

		// EmissionMetallic -> DXGI_FORMAT_R8G8B8A8_UNORM
		// RGB -> Pixel emission | A -> Pixel metallic
		RenderTexture m_emissionMetallic;

		// DepthStencil -> DXGI_FORMAT_D24_UNORM_S8_UINT
		// RGB -> Pixel Depth | A -> Stencil
		DepthTexture m_depthStencil;

		// Resources pointer arrays for quick binding
		ID3D11RenderTargetView*   m_renderTargetViews[4] = { nullptr };
		ID3D11ShaderResourceView* m_shaderResourceViews[4] = { nullptr };
	};
}