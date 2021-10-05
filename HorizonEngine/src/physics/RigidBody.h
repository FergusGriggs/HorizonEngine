#pragma once

#include "ParticleModel.h"
#include <algorithm>

class RigidBody : public ParticleModel
{
private:
	XMMATRIX inertiaTensor;
	XMMATRIX inverseInertiaTensor;

	float angularDrag;

	XMVECTOR netTorque;
	XMVECTOR angularAcceleration;
	XMVECTOR angularVelocity;

public:
	RigidBody();

	void Update(float deltaTime) override;

	void AddTorque(XMVECTOR relativePosition, XMVECTOR force);
	void AddForceSplit(XMVECTOR position, XMVECTOR force);

	void ComputeAngularAcceleration();
	void ComputeAngluarVelocity(float deltaTime);
	void ComputeOrientation(float deltaTime);

	void ApplyAngularDrag(float deltaTime);

	void ComputeSphereIntertiaTensor(float radius);
	void ComputeShellIntertiaTensor(float radius);
	void ComputeBoxIntertiaTensor(float sizeX, float sizeY, float sizeZ);

	XMVECTOR GetForceAtRelativePosition(XMVECTOR relativePosition);
};

