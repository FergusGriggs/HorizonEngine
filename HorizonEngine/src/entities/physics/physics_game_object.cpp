#include "physics_game_object.h"

namespace hrzn::entity::physics
{
	PhysicsGameObject::PhysicsGameObject() :
		RenderableGameObject(),
		m_rigidBody(),
		m_worldSpaceBoundingBox()
	{
		m_type = GameObjectType::ePhysics;

		m_rigidBody.setTransformReference(&m_transform);
	}

	PhysicsGameObject::~PhysicsGameObject()
	{
	}

	bool PhysicsGameObject::initialize(std::string label, const std::string& filePath, ID3D11Device* device, ID3D11DeviceContext* deviceContext)
	{
		if (RenderableGameObject::initialize(label, filePath, device, deviceContext))
		{
			BoundingBox boundingBox = m_model->getBoundingBox();
			m_rigidBody.computeBoxIntertiaTensor(boundingBox.Extents.x * 2.0f, boundingBox.Extents.y * 2.0f, boundingBox.Extents.z * 2.0f);

			return true;
		}

		return false;
	}

	void PhysicsGameObject::setMass(float mass)
	{
		m_rigidBody.setMass(mass);
	}

	void PhysicsGameObject::setDrag(float drag)
	{
		m_rigidBody.setDrag(drag);
	}

	float PhysicsGameObject::getMass()
	{
		return m_rigidBody.getMass();
	}

	float PhysicsGameObject::getDrag()
	{
		return m_rigidBody.getDrag();
	}

	RigidBody* PhysicsGameObject::getRigidBody()
	{
		return &m_rigidBody;
	}

	void PhysicsGameObject::update(float deltaTime)
	{
		GameObject::update(deltaTime);

		m_rigidBody.update(deltaTime);

		updateWorldSpaceBoundingBox();
	}

	void PhysicsGameObject::updateWorldSpaceBoundingBox()
	{
		/*BoundingBox modelSpaceBoundingBox = model->GetBoundingBox();

		XMFLOAT3 corners[8];

		modelSpaceBoundingBox.GetCorners(corners);

		XMFLOAT3 position = transform.GetPositionFloat3();
		XMMATRIX transform = transform.GetRotationMatrix() * XMMatrixTranslation(position.x, position.y, position.z);

		for (int i = 0; i < 8; ++i)
		{
			XMVECTOR vectorCorner = XMLoadFloat3(&(corners[i]));

			vectorCorner = XMVector3Transform(vectorCorner, transform);

			XMStoreFloat3(&(corners[i]), vectorCorner);
		}

		BoundingBox::CreateFromPoints(worldSpaceBoundingBox, 8, corners, sizeof(XMFLOAT3));*/

		BoundingBox modelSpaceBoundingBox = m_model->getBoundingBox();
		m_worldSpaceBoundingBox.Extents = modelSpaceBoundingBox.Extents;
		m_worldSpaceBoundingBox.Center = m_transform.getPositionFloat3();
		XMStoreFloat4(&m_worldSpaceBoundingBox.Orientation, m_transform.getOrientation());
	}

	BoundingOrientedBox PhysicsGameObject::getWorldSpaceBoundingBox()
	{
		return m_worldSpaceBoundingBox;
	}
}
