#include "PhysicsGameObject.h"

PhysicsGameObject::PhysicsGameObject()
{
	this->type = GameObjectType::PHYSICS;

	this->particleModel.SetTransformReference(&this->transform);
}

bool PhysicsGameObject::Initialize(std::string label, const std::string& filePath, ID3D11Device* device, ID3D11DeviceContext* deviceContext, ResourceManager* resourceManager)
{
	return RenderableGameObject::Initialize(label, filePath, device, deviceContext, resourceManager);
}

void PhysicsGameObject::SetMass(float mass)
{
	this->particleModel.SetMass(mass);
}

void PhysicsGameObject::SetDrag(float drag)
{
	this->particleModel.SetDrag(drag);
}

float PhysicsGameObject::GetMass()
{
	return this->particleModel.GetMass();
}

float PhysicsGameObject::GetDrag()
{
	return this->particleModel.GetDrag();
}

ParticleModel* PhysicsGameObject::GetParticleModel()
{
	return &this->particleModel;
}

void PhysicsGameObject::Update(float deltaTime)
{
	GameObject::Update(deltaTime);

	this->particleModel.Update(deltaTime);
}
