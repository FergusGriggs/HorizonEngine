
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

		bool  initialize(std::string label, const std::string& filePath);

		template <class T>
		void  draw(const XMMATRIX& viewProjectionMatrix, gfx::ConstantBuffer<T>* vertexShaderCB, bool bindTextures = true) const;

		float getRayIntersectDist(XMVECTOR rayOrigin, XMVECTOR rayDirection) const;

		const XMFLOAT3&    getScale() const;
		const gfx::Model&  getModel() const;

		void        setScale(XMFLOAT3 scale);

	protected:
		gfx::Model* m_model;
		gfx::Model* m_axisModel;

		XMFLOAT3    m_scale;
	};
}


namespace hrzn::entity
{
	template<class T>
	inline void RenderableGameObject::draw(const XMMATRIX& viewProjectionMatrix, gfx::ConstantBuffer<T>* vertexShaderCB, bool bindTextures) const
	{
		XMFLOAT3 objectPosition = m_transform.getPositionFloat3();

		if (m_floating)
		{
			objectPosition = XMFLOAT3(objectPosition.x + m_floatOffset.x, m_floatOffset.y, objectPosition.z + m_floatOffset.z);
		}

		if (m_type == GameObject::Type::eRenderable)
		{
			m_model->draw(XMMatrixScaling(m_scale.x, m_scale.y, m_scale.z) * m_transform.getRotationMatrix() * XMMatrixTranslation(objectPosition.x, objectPosition.y, objectPosition.z), viewProjectionMatrix, vertexShaderCB, bindTextures);
		}
		else
		{
			m_model->draw(m_transform.getRotationMatrix() * XMMatrixTranslation(objectPosition.x, objectPosition.y, objectPosition.z), viewProjectionMatrix, vertexShaderCB, bindTextures);
		}
	}
}