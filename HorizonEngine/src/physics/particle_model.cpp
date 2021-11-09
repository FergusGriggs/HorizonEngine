#include "particle_model.h"

namespace hrzn::physics
{
	ParticleModel::ParticleModel() :
		m_type(ParticleModelType::eBase),

		m_transformReference(nullptr),

		m_velocity(XMFLOAT3(0.0f, 0.0f, 0.0f)),
		m_acceleration(XMFLOAT3(0.0f, 0.0f, 0.0f)),
		m_netForce(XMFLOAT3(0.0f, 0.0f, 0.0f)),

		m_thrustForces(),

		m_isStatic(true),

		m_mass(50.0f),
		m_drag(0.5f),
		m_floorHeight(-2.0f)
	{
	}

	ParticleModelType ParticleModel::getType()
	{
		return m_type;
	}

	void ParticleModel::setIsStatic(bool isStatic)
	{
		m_isStatic = isStatic;
	}

	void ParticleModel::setMass(float mass)
	{
		m_mass = mass;
	}

	void ParticleModel::setDrag(float drag)
	{
		m_drag = drag;
	}

	bool ParticleModel::isStatic()
	{
		return m_isStatic;
	}

	bool* ParticleModel::isStaticPtr()
	{
		return &m_isStatic;
	}

	float ParticleModel::getMass()
	{
		return m_mass;
	}

	float ParticleModel::getDrag()
	{
		return m_drag;
	}

	void ParticleModel::computeAcceleration(float deltaTime)
	{
		XMVECTOR acceleration = XMLoadFloat3(&m_netForce) / m_mass;
		XMStoreFloat3(&m_acceleration, acceleration);
	}

	void ParticleModel::computeVelocity(float deltaTime)
	{
		XMVECTOR newVelocity = getVelocityVector() + XMLoadFloat3(&m_acceleration) * deltaTime;
		XMStoreFloat3(&m_velocity, newVelocity);
	}

	void ParticleModel::computePosition(float deltaTime)
	{
		XMVECTOR displacement = getVelocityVector() * deltaTime - 0.5f * getAccelerationVector() * pow(deltaTime, 2); // s = vt - 0.5at^2

		displacement *= 2.0f; //account for scale differences

		m_transformReference->adjustPosition(displacement);
	}

	void ParticleModel::applyThrustForces(float deltaTime)
	{
		std::vector<int> deadForces;

		for (int i = 0; i < m_thrustForces.size(); ++i)
		{
			addForce(m_thrustForces[i].first);

			m_thrustForces[i].second -= deltaTime;

			if (m_thrustForces[i].second <= 0.0f)
			{
				deadForces.push_back(i);
			}
		}

		// Remove dead forces
		for (int i = deadForces.size() - 1; i >= 0; --i)
		{
			m_thrustForces.erase(m_thrustForces.begin() + deadForces[i]);
		}
	}

	void ParticleModel::addThrust(const XMVECTOR& force, float duration)
	{
		m_thrustForces.push_back(std::make_pair(XMFLOAT3(0.0f, 0.0f, 0.0f), duration));
		XMStoreFloat3(&(m_thrustForces.back().first), force);
	}

	void ParticleModel::addThrust(const XMFLOAT3& force, float duration)
	{
		m_thrustForces.push_back(std::make_pair(force, duration));
	}

	void ParticleModel::addForce(const XMVECTOR& force)
	{
		XMStoreFloat3(&m_netForce, XMLoadFloat3(&m_netForce) + force);
	}

	void ParticleModel::addForce(const XMFLOAT3& force)
	{
		addForce(XMLoadFloat3(&force));
	}

	void ParticleModel::update(float deltaTime)
	{
		if (!m_isStatic)
		{
			// Forces
			applyGravity(deltaTime);
			applyThrustForces(deltaTime);
			//ApplyDragTurbulentFlow();

			computeAcceleration(deltaTime);
			computeVelocity(deltaTime);
			applyDrag(deltaTime);
			computePosition(deltaTime);

			// Collision
			checkFloorHeight();
		}

		// Reset net force
		m_netForce = XMFLOAT3(0.0f, 0.0f, 0.0f);
	}

	void ParticleModel::applyGravity(float deltaTime)
	{
		addForce(XMFLOAT3(0.0f, GRAVITY * m_mass, 0.0f));
	}

	void ParticleModel::applyDrag(float deltaTime)
	{
		float dragThisUpdate = 1.0f - (deltaTime * m_drag);
		setVelocity(getVelocityVector() * dragThisUpdate);
	}

	void ParticleModel::applyDragTurbulentFlow()
	{
		// Calculate magnitude of velocity
		float velocityMagnitude = XMVectorGetX(XMVector3Length(getVelocityVector()));
		// Calculate unit vector of velocity
		XMVECTOR unitVelocity = XMVector3Normalize(getVelocityVector());

		// Calculate magnitude of drag force
		float dragMagnitude = m_drag * pow(velocityMagnitude, 2);

		// Calculate net force difference
		addForce(-1.0f * dragMagnitude * unitVelocity);
	}

	void ParticleModel::applyDragLaminarFlow()
	{
		addForce(-1.0f * m_drag * getVelocityVector());
	}

	void ParticleModel::setTransformReference(Transform* transformReference)
	{
		m_transformReference = transformReference;
	}

	Transform* ParticleModel::getTransformReference()
	{
		return m_transformReference;
	}

	void ParticleModel::checkFloorHeight()
	{
		XMFLOAT3 position = m_transformReference->getPositionFloat3();
		if (position.y < m_floorHeight)
		{
			setVelocity(XMFLOAT3(getVelocityFloat3().x, -0.3f * getVelocityFloat3().y, getVelocityFloat3().z));
			m_transformReference->setPosition(XMVectorSet(position.x, m_floorHeight, position.z, 1.0f));
		}

		if (position.x < -30.0f || position.x > 30.0f)
		{
			setVelocity(XMFLOAT3(-getVelocityFloat3().x, getVelocityFloat3().y, getVelocityFloat3().z));
		}
		if (position.z < -30.0f || position.z > 30.0f)
		{
			setVelocity(XMFLOAT3(getVelocityFloat3().x, getVelocityFloat3().y, -getVelocityFloat3().z));
		}
	}

	void ParticleModel::setVelocity(const XMVECTOR& velocity)
	{
		XMStoreFloat3(&m_velocity, velocity);
	}

	void ParticleModel::setVelocity(const XMFLOAT3& velocity)
	{
		m_velocity = velocity;
	}

	const XMVECTOR& ParticleModel::getVelocityVector()
	{
		return XMLoadFloat3(&m_velocity);
	}

	const XMFLOAT3& ParticleModel::getVelocityFloat3()
	{
		return m_velocity;
	}

	const XMVECTOR& ParticleModel::getAccelerationVector()
	{
		return XMLoadFloat3(&m_acceleration);
	}

	const XMFLOAT3& ParticleModel::getAccelerationFloat3()
	{
		return m_acceleration;
	}
}