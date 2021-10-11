
//Child class of GameObject which stores an additional data to allow the object to be rendered and transformed

#pragma once

#include "game_object.h"

#include "../graphics/data/resource_manager.h"

namespace hrzn::entity
{
	class RenderableGameObject : public GameObject
	{
	public:
		RenderableGameObject();

		bool  initialize(std::string label, const std::string& filePath, ID3D11Device* device, ID3D11DeviceContext* deviceContext);
		void  draw(const XMMATRIX& viewProjectionMatrix, gfx::ConstantBuffer<gfx::VertexShaderCB>* cb_vs_vertexShader, bool bindTextures = true);

		float getRayIntersectDist(XMVECTOR rayOrigin, XMVECTOR rayDirection);

		XMFLOAT3    getScale();
		gfx::Model* getModel();

		void        setScale(XMFLOAT3 scale);

	protected:
		gfx::Model* m_model;
		gfx::Model* m_axisModel;

		XMFLOAT3    m_scale;
	};
}

