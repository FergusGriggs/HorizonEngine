
//Function implementations for the RenderableGameObject class

#include "renderable_game_object.h"

#include "../graphics/data/resource_manager.h"
#include "../graphics/data/vertex_types.h"
#include "../graphics/graphics_handler.h"

namespace hrzn::entity
{
	RenderableGameObject::RenderableGameObject() :
		GameObject(),

		m_model(nullptr),

		m_scale(XMFLOAT3(1.0f, 1.0f, 1.0f))
	{
		m_type = GameObject::Type::eRenderable;
	}

	bool RenderableGameObject::initialise(std::string label, const std::string& filePath)
	{
		m_label = label;

		m_model = gfx::ResourceManager::it().getModelPtr<gfx::FancyLitVertex>(filePath);

		if (m_model == nullptr)
		{
			return false;
		}

		return true;
	}

	void RenderableGameObject::draw(bool bindPSData) const
	{
		XMFLOAT3 objectPosition = m_transform.getPositionFloat3();

		if (m_floating)
		{
			objectPosition = XMFLOAT3(objectPosition.x + m_floatOffset.x, m_floatOffset.y, objectPosition.z + m_floatOffset.z);
		}

		if (m_type == GameObject::Type::eRenderable)
		{
			m_model->draw(XMMatrixScaling(m_scale.x, m_scale.y, m_scale.z) * m_transform.getRotationMatrix() * XMMatrixTranslation(objectPosition.x, objectPosition.y, objectPosition.z), bindPSData);
		}
		else
		{
			m_model->draw(m_transform.getRotationMatrix() * XMMatrixTranslation(objectPosition.x, objectPosition.y, objectPosition.z), bindPSData);
		}
	}

	float RenderableGameObject::getRayIntersectDist(XMVECTOR rayOrigin, XMVECTOR rayDirection) const
	{
		BoundingBox objectBoundingBox = m_model->getBoundingBox();
		XMFLOAT3 objectPosition = m_transform.getPositionFloat3();
		objectBoundingBox.Center.x += objectPosition.x;
		objectBoundingBox.Center.y += objectPosition.y;
		objectBoundingBox.Center.z += objectPosition.z;

		XMFLOAT4 orientationFloat4;
		XMStoreFloat4(&orientationFloat4, m_transform.getOrientation());

		BoundingOrientedBox orientedObjectBoundingBox = BoundingOrientedBox(objectBoundingBox.Center, objectBoundingBox.Extents, orientationFloat4);

		float rayDistance;
		if (orientedObjectBoundingBox.Intersects(rayOrigin, rayDirection, rayDistance))
		{
			XMMATRIX inverseRotationMatrix = XMMatrixInverse(nullptr, m_transform.getRotationMatrix());

			XMVECTOR rayOriginLocalSpace = XMVector3Transform(rayOrigin - m_transform.getPositionVector(), inverseRotationMatrix);
			XMVECTOR rayDirectionLocalSpace = XMVector3Transform(rayDirection, inverseRotationMatrix);

			if (getModel().rayInersectAllFaces(rayOriginLocalSpace, rayDirectionLocalSpace, &rayDistance))
			{
				return rayDistance;
			}
		}

		return FLT_MAX;
	}

	const XMFLOAT3& RenderableGameObject::getScale() const
	{
		return m_scale;
	}

	void RenderableGameObject::setScale(XMFLOAT3 scale)
	{
		m_scale = scale;
	}

	const gfx::Model& RenderableGameObject::getModel() const
	{
		return *m_model;
	}

	gfx::Model& RenderableGameObject::getWritableModel()
	{
		return *m_model;
	}
}
