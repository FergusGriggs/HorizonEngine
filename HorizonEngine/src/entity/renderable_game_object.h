
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

		bool  initialize(std::string label, const std::string& filePath);

		template <class ConstantBufferType>
		void  draw(gfx::ConstantBuffer<ConstantBufferType>* vertexShaderCB, bool bindPSData = true) const;

		float getRayIntersectDist(XMVECTOR rayOrigin, XMVECTOR rayDirection) const;

		const XMFLOAT3&    getScale() const;
		const gfx::Model&  getModel() const;

		void        setScale(XMFLOAT3 scale);

	protected:
		gfx::Model* m_model;

		XMFLOAT3    m_scale;
	};
}


namespace hrzn::entity
{
	template<class ConstantBufferType>
	inline void RenderableGameObject::draw(gfx::ConstantBuffer<ConstantBufferType>* vertexShaderCB, bool bindPSData) const
	{
		XMFLOAT3 objectPosition = m_transform.getPositionFloat3();

		if (m_floating)
		{
			objectPosition = XMFLOAT3(objectPosition.x + m_floatOffset.x, m_floatOffset.y, objectPosition.z + m_floatOffset.z);
		}

		if (m_type == GameObject::Type::eRenderable)
		{
			m_model->draw(XMMatrixScaling(m_scale.x, m_scale.y, m_scale.z) * m_transform.getRotationMatrix() * XMMatrixTranslation(objectPosition.x, objectPosition.y, objectPosition.z), vertexShaderCB, bindPSData);
		}
		else
		{
			m_model->draw(m_transform.getRotationMatrix() * XMMatrixTranslation(objectPosition.x, objectPosition.y, objectPosition.z), vertexShaderCB, bindPSData);
		}
	}
}