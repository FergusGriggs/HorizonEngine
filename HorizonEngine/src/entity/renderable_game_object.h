
//Child class of GameObject which stores an additional data to allow the object to be rendered and transformed

#pragma once

#include "game_object.h"

#include "../graphics/buffers/constant_buffer.h"
#include "../graphics/data/model.h"

namespace hrzn::entity
{
	class RenderableGameObject : public GameObject
	{
	public:
		RenderableGameObject();

		virtual bool initialise(std::string label, const std::string& filePath);

		void  draw(bool bindPSData = true) const;

		float getRayIntersectDist(XMVECTOR rayOrigin, XMVECTOR rayDirection) const;

		const XMFLOAT3&    getScale() const;
		const gfx::Model&  getModel() const;
		gfx::Model&        getWritableModel();

		void        setScale(XMFLOAT3 scale);

	protected:
		gfx::Model* m_model;

		XMFLOAT3    m_scale;
	};
}