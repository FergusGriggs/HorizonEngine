
//Function implementations for the RenderableGameObject class

#include "renderable_game_object.h"

namespace hrzn::entity
{
	RenderableGameObject::RenderableGameObject() :
		GameObject(),

		m_model(nullptr),
		m_axisModel(nullptr),

		m_scale(XMFLOAT3(1.0f, 1.0f, 1.0f))
	{
		m_type = GameObject::Type::eRenderable;
	}

	bool RenderableGameObject::initialize(std::string label, const std::string& filePath)
	{
		m_label = label;

		m_model = gfx::ResourceManager::it().getModelPtr(filePath);

		if (m_model == nullptr)
		{
			return false;
		}

		return true;
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
}
