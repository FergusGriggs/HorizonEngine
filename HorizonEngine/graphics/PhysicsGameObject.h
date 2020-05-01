#pragma once

#include "RenderableGameObject.h"
#include "../physics/RigidBody.h"

class PhysicsGameObject : public RenderableGameObject
{
public:
	PhysicsGameObject();
	~PhysicsGameObject();

	bool Initialize(std::string label, const std::string& filePath, ID3D11Device* device, ID3D11DeviceContext* deviceContext, ResourceManager* resourceManager);

	void SetMass(float mass);
	void SetDrag(float drag);

	float GetMass();
	float GetDrag();

	RigidBody* GetRigidBody();

	void Update(float deltaTime) override;
	void UpdateWorldSpaceBoundingBox();
	BoundingOrientedBox GetWorldSpaceBoundingBox();

private:
	RigidBody rigidBody;

	BoundingOrientedBox worldSpaceBoundingBox;
};

