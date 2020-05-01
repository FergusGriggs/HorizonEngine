#include "ParticleModel.h"

void ParticleModel::Update(float deltaTime)
{
	if (!this->isStatic)
	{
		// Forces
		this->ApplyGravity(deltaTime);
		this->ApplyThrustForces(deltaTime);
		//this->ApplyDragTurbulentFlow();

		this->ComputeAcceleration(deltaTime);
		this->ComputeVelocity(deltaTime);
		this->ApplyDrag(deltaTime);
		this->ComputePosition(deltaTime);

		// Collision
		this->CheckAbsoluteFloorHeight();
	}

	// Reset net force
	this->netForce = XMVectorSet(0.0f, 0.0f, 0.0f, 0.0f);
}

void ParticleModel::ApplyGravity(float deltaTime)
{
	this->netForce -= XMVectorSet(0.0f, GRAVITY * this->mass, 0.0f, 0.0f);
}

void ParticleModel::ApplyDrag(float deltaTime)
{
	this->velocity *= 1.0f - (deltaTime * drag);
}

void ParticleModel::ApplyDragTurbulentFlow()
{
	// Calculate magnitude of velocity
	float velocityMagnitude = XMVectorGetX(XMVector3Length(this->velocity));
	// Calculate unit vector of velocity
	XMVECTOR unitVelocity = XMVector3Normalize(this->velocity);

	// Calculate magnitude of drag force
	float dragMagnitude = drag * pow(velocityMagnitude, 2);

	// Calculate net force difference
	this->netForce -= dragMagnitude * unitVelocity;
}

void ParticleModel::ApplyDragLaminarFlow()
{
	this->netForce -= drag * velocity;
}

ParticleModel::ParticleModel()
{
	this->velocity = XMVectorZero();
	this->acceleration = XMVectorZero();
	this->netForce = XMVectorZero();
	this->mass = 50.0f;
	this->drag = 0.5f;
	this->transformReference = nullptr;
	this->isStatic = true;
	this->absoluteFloorHeight = -2.0f;
	this->type = ParticleModelType::BASE;
}

void ParticleModel::SetIsStatic(bool isStatic)
{
	this->isStatic = isStatic;
}

void ParticleModel::SetMass(float mass)
{
	this->mass = mass;
}

void ParticleModel::SetDrag(float drag)
{
	this->drag = drag;
}

float ParticleModel::GetMass()
{
	return this->mass;
}

float ParticleModel::GetDrag()
{
	return this->drag;
}

bool ParticleModel::IsStatic()
{
	return this->isStatic;
}

bool* ParticleModel::IsStaticPtr()
{
	return &this->isStatic;
}

void ParticleModel::ComputeAcceleration(float deltaTime)
{
	this->acceleration = this->netForce / this->mass;
}

void ParticleModel::ComputeVelocity(float deltaTime)
{
	this->velocity += this->acceleration * deltaTime; //v = u + at
}

void ParticleModel::ComputePosition(float deltaTime)
{
	XMVECTOR displacement = this->velocity * deltaTime - 0.5f * this->acceleration * pow(deltaTime, 2); // s = vt - 0.5at^2

	displacement *= 2.0f; //account for scale differences

	this->transformReference->AdjustPosition(displacement);
}

void ParticleModel::ApplyThrustForces(float deltaTime)
{
	std::vector<int> deadForces;

	for (int i = 0; i < this->thrustForces.size(); ++i)
	{
		this->netForce += this->thrustForces[i].first;

		this->thrustForces[i].second -= deltaTime;

		if (this->thrustForces[i].second <= 0.0f)
		{
			deadForces.push_back(i);
		}
	}

	// Remove dead forces
	for (int i = deadForces.size() - 1; i >= 0; --i)
	{
		this->thrustForces.erase(this->thrustForces.begin() + deadForces[i]);
	}
}

void ParticleModel::AddThrust(XMVECTOR force, float duration)
{
	this->thrustForces.push_back(std::make_pair(force, duration));
}

void ParticleModel::AddForce(XMVECTOR force)
{
	this->netForce += force;
}

void ParticleModel::SetTransformReference(Transform* transformReference)
{
	this->transformReference = transformReference;
}

Transform* ParticleModel::GetTransformReference()
{
	return this->transformReference;
}

void ParticleModel::CheckAbsoluteFloorHeight()
{
	XMFLOAT3 position = this->transformReference->GetPositionFloat3();
	if (position.y < this->absoluteFloorHeight)
	{
		this->velocity = XMVectorSet(XMVectorGetX(this->velocity), -0.3f * XMVectorGetY(this->velocity), XMVectorGetZ(this->velocity), 0.0f);
		this->transformReference->SetPosition(XMVectorSet(position.x, this->absoluteFloorHeight, position.z, 1.0f));
	}

	if (position.x < -30.0f || position.x > 30.0f)
	{
		this->velocity = XMVectorSet(-XMVectorGetX(this->velocity), XMVectorGetY(this->velocity), XMVectorGetZ(this->velocity), 0.0f);
	}
	if (position.z < -30.0f || position.z > 30.0f)
	{
		this->velocity = XMVectorSet(XMVectorGetX(this->velocity), XMVectorGetY(this->velocity), -XMVectorGetZ(this->velocity), 0.0f);
	}
}

const XMVECTOR& ParticleModel::GetVelocity()
{
	return this->velocity;
}

void ParticleModel::SetVelocity(const XMVECTOR& velocity)
{
	this->velocity = velocity;
}

ParticleModelType ParticleModel::GetType()
{
	return this->type;
}
