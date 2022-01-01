#pragma once

#include <d3d11.h>
#include <DirectXMath.h>

#include "data/model.h"

#include "buffers/geometry_buffer.h"

#include "post_process/post_process.h"

namespace hrzn::gfx
{
	class ImageRenderer
	{
	public:
		ImageRenderer();
		~ImageRenderer();

		void initialise(CD3D11_VIEWPORT viewport, ID3D11DepthStencilState* depthStencilState,
			ID3D11RasterizerState* rasterizerState);

		void setViewport(CD3D11_VIEWPORT viewport, ID3D11Device* device);
		void setRasterizerState(ID3D11RasterizerState* rasterizerState);

		void addPostProcess(PostProcess* postProcess);
		std::vector<PostProcess*>& getPostProcesses();
		void clearPostProcesses();
		

		void render(const DirectX::XMVECTOR& eyePos, const DirectX::XMVECTOR& eyeFacing, const DirectX::XMMATRIX& viewMatrix, const DirectX::XMMATRIX& projectionMatrix);

		RenderTexture&  getFinalImage();
		GeometryBuffer* getGBuffer();

		void release();

	private:
		void internalRenderStandard(const DirectX::XMVECTOR& eyePos, const DirectX::XMVECTOR& eyeFacing, const DirectX::XMMATRIX& viewMatrix, const DirectX::XMMATRIX& projectionMatrix);
		void internalRenderDeferredShading(const DirectX::XMVECTOR& eyePos, const DirectX::XMVECTOR& eyeFacing, const DirectX::XMMATRIX& viewMatrix, const DirectX::XMMATRIX& projectionMatrix);

	private:
		CD3D11_VIEWPORT          m_viewport;
		GeometryBuffer           m_geometryBuffer;

		RenderTexture            m_finalImage;

		ID3D11DepthStencilState* m_depthStencilState;
		ID3D11RasterizerState*   m_rasterizerState;

		Model*                   m_quadModel;

		std::vector<PostProcess*> m_postProcesses;

	};
}
