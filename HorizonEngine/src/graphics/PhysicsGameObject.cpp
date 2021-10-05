#include "PhysicsGameObject.h"

PhysicsGameObject::PhysicsGameObject()
{
	this->type = GameObjectType::PHYSICS;

	this->rigidBody.SetTransformReference(&this->transform);
}

PhysicsGameObject::~PhysicsGameObject()
{
}

bool PhysicsGameObject::Initialize(std::string label, const std::string& filePath, ID3D11Device* device, ID3D11DeviceContext* deviceContext, ResourceManager* resourceManager)
{
	if (RenderableGameObject::Initialize(label, filePath, device, deviceContext, resourceManager))
	{
		BoundingBox boundingBox = this->model->GetBoundingBox();
		this->rigidBody.ComputeBoxIntertiaTensor(boundingBox.Extents.x * 2.0f, boundingBox.Extents.y * 2.0f, boundingBox.Extents.z * 2.0f);

		return true;
	}

	return false;
}

void PhysicsGameObject::SetMass(float mass)
{
	this->rigidBody.SetMass(mass);
}

void PhysicsGameObject::SetDrag(float drag)
{
	this->rigidBody.SetDrag(drag);
}

float PhysicsGameObject::GetMass()
{
	return this->rigidBody.GetMass();
}

float PhysicsGameObject::GetDrag()
{
	return this->rigidBody.GetDrag();
}

RigidBody* PhysicsGameObject::GetRigidBody()
{
	return &this->rigidBody;
}

void PhysicsGameObject::Update(float deltaTime)
{
	GameObject::Update(deltaTime);

	this->rigidBody.Update(deltaTime);

	UpdateWorldSpaceBoundingBox();
}

void PhysicsGameObject::UpdateWorldSpaceBoundingBox()
{
	BoundingBox modelSpaceBoundingBox = this->model->GetBoundingBox();

	XMFLOAT3 corners[8];

	modelSpaceBoundingBox.GetCorners(corners);

	for (int i = 0; i < 8; ++i)
	{
		XMVECTOR vectorCorner = XMLoadFloat3(&(corners[i]));

		vectorCorner = XMVector3Transform(vectorCorner, this->transform.GetRotationMatrix());

		XMStoreFloat3(&(corners[i]), vectorCorner);
	}

	BoundingBox::CreateFromPoints(this->worldSpaceBoundingBox, 8, corners, sizeof(XMFLOAT3));
}