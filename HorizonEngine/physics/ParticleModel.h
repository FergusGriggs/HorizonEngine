#pragma once

#include <DirectXMath.h>

#include "../graphics/Transform.h"
#include <vector>

using namespace DirectX;

const float GRAVITY = 9.81f;
//const float GRAVITY = 0.0981f;

class ParticleModel
{
private:
	bool isStatic;
	float mass;
	float drag;

	float absoluteFloorHeight;

	Transform* transformReference;

	XMVECTOR velocity;
	XMVECTOR acceleration;
	XMVECTOR netForce;

	std::vector<std::pair<XMVECTOR, float>> thrustForces;

public:
	ParticleModel();

	void SetIsStatic(bool isStatic);
	void SetMass(float mass);
	void SetDrag(float drag);

	float GetMass();
	float GetDrag();
	bool IsStatic();
	bool* IsStaticPtr();

	void ComputeAcceleration(float deltaTime);
	void ComputeVelocity(float deltaTime);
	void ComputePosition(float deltaTime);
	void ApplyThrustForces(float deltaTime);

	void AddThrust(XMVECTOR force, float duration);
	void Update(float deltaTime);

	void ApplyGravity(float deltaTime);
	void ApplyDrag(float deltaTime);
	void ApplyDragTurbulentFlow();
	void ApplyDragLaminarFlow();

	void SetTransformReference(Transform* transformReference);

	void CheckAbsoluteFloorHeight();
};

