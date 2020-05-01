#include "RigidBody.h"

RigidBody::RigidBody()
{
	ParticleModel::ParticleModel();

	this->netTorque = XMVectorZero();
	this->angularAcceleration = XMVectorZero();
	this->angularVelocity = XMVectorZero();
	this->angularDrag = 1.0f;

	this->ComputeBoxIntertiaTensor(1.0f, 1.0f, 1.0f);

	this->type = ParticleModelType::RIGID_BODY;
}

void RigidBody::Update(float deltaTime)
{
	ParticleModel::Update(deltaTime);

	if (!isStatic)
	{
		ComputeAngularAcceleration();
		ComputeAngluarVelocity(deltaTime);
		ApplyAngularDrag(deltaTime);
		ComputeOrientation(deltaTime);

		this->netTorque = XMVectorZero();
	}
}

void RigidBody::AddTorque(XMVECTOR relativePosition, XMVECTOR force)
{
	XMMATRIX inverseRotation = XMMatrixInverse(nullptr, this->transformReference->GetRotationMatrix());
	this->netTorque += XMVector3Transform(XMVector3Cross(relativePosition, force), inverseRotation);
}

void RigidBody::AddForceSplit(XMVECTOR position, XMVECTOR force)
{
	XMVECTOR relativePosition = position - this->transformReference->GetPositionVector();

	XMVECTOR toCentreOfMass = XMVector3Normalize(-relativePosition);
	XMVECTOR forceDirection = XMVector3Normalize(force);

	float dotProduct = std::max(XMVectorGetX(XMVector3Dot(toCentreOfMass, forceDirection)), 0.0f);

	AddForce(force);// * dotProduct
	AddTorque(relativePosition, force * (1.0f - dotProduct));
}

void RigidBody::ComputeAngularAcceleration()
{
	this->angularAcceleration = XMVector3Transform(this->netTorque, this->inverseInertiaTensor);
}

void RigidBody::ComputeAngluarVelocity(float deltaTime)
{
	this->angularVelocity += this->angularAcceleration * deltaTime; //v = u + at
}

void RigidBody::ComputeOrientation(float deltaTime)
{
	XMVECTOR orientationChange = this->angularVelocity * deltaTime;
	XMVECTOR oldOrientation = this->transformReference->GetOrientation();
	XMVECTOR newOrientation = XMQuaternionNormalize(XMQuaternionMultiply(orientationChange, oldOrientation));

	this->transformReference->SetOrientationQuaternion(newOrientation);
}

void RigidBody::ApplyAngularDrag(float deltaTime)
{
	this->angularVelocity *= 1.0f - (this->angularDrag * deltaTime);
}

void RigidBody::ComputeSphereIntertiaTensor(float radius)
{
	// Initialize to identity matrix
	this->inertiaTensor = XMMatrixIdentity();

	// Set matrix values
	this->inertiaTensor.r[0].m128_f32[0] = 0.4f * this->mass * pow(radius, 2);
	this->inertiaTensor.r[1].m128_f32[1] = 0.4f * this->mass * pow(radius, 2);
	this->inertiaTensor.r[2].m128_f32[2] = 0.4f * this->mass * pow(radius, 2);

	// Compute inverse inertia tensor
	XMVECTOR determinant = XMMatrixDeterminant(this->inertiaTensor);
	this->inverseInertiaTensor = XMMatrixInverse(&determinant, this->inertiaTensor);
}

void RigidBody::ComputeShellIntertiaTensor(float radius)
{
	// Initialize to identity matrix
	this->inertiaTensor = XMMatrixIdentity();

	// Set matrix values
	this->inertiaTensor.r[0].m128_f32[0] = 0.666666f * this->mass * pow(radius, 2);
	this->inertiaTensor.r[1].m128_f32[1] = 0.666666f * this->mass * pow(radius, 2);
	this->inertiaTensor.r[2].m128_f32[2] = 0.666666f * this->mass * pow(radius, 2);

	// Compute inverse inertia tensor
	XMVECTOR determinant = XMMatrixDeterminant(this->inertiaTensor);
	this->inverseInertiaTensor = XMMatrixInverse(&determinant, this->inertiaTensor);
}

void RigidBody::ComputeBoxIntertiaTensor(float sizeX, float sizeY, float sizeZ)
{
	// Initialize to identity matrix
	this->inertiaTensor = XMMatrixIdentity();

	// Square box bounds
	float sizeXsquared = pow(sizeX, 2);
	float sizeYsquared = pow(sizeY, 2);
	float sizeZsquared = pow(sizeZ, 2);

	// Set matrix values
	this->inertiaTensor.r[0].m128_f32[0] = 0.083333f * this->mass * (sizeYsquared + sizeZsquared);
	this->inertiaTensor.r[1].m128_f32[1] = 0.083333f * this->mass * (sizeXsquared + sizeZsquared);
	this->inertiaTensor.r[2].m128_f32[2] = 0.083333f * this->mass * (sizeXsquared + sizeYsquared);

	// Compute inverse inertia tensor
	XMVECTOR determinant = XMMatrixDeterminant(this->inertiaTensor);
	this->inverseInertiaTensor = XMMatrixInverse(&determinant, this->inertiaTensor);
}
