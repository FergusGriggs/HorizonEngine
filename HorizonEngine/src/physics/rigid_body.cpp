#include "rigid_body.h"

namespace hrzn::physics
{
	RigidBody::RigidBody() :
		ParticleModel(),

		m_netTorque(XMVectorZero()),
		m_angularAcceleration(XMVectorZero()),
		m_angularVelocity(XMVectorZero()),

		m_inertiaTensor(XMMatrixIdentity()),
		m_inverseInertiaTensor(XMMatrixIdentity()),

		m_angularDrag(1.0f)
	{
		computeBoxIntertiaTensor(1.0f, 1.0f, 1.0f);

		m_type = ParticleModelType::eRigidBody;
	}

	void RigidBody::update(float deltaTime)
	{
		ParticleModel::update(deltaTime);

		if (!m_isStatic)
		{
			computeAngularAcceleration();
			computeAngluarVelocity(deltaTime);
			applyAngularDrag(deltaTime);
			computeOrientation(deltaTime);

			m_netTorque = XMVectorZero();
		}
	}

	void RigidBody::addTorque(XMVECTOR relativePosition, XMVECTOR force)
	{
		XMMATRIX inverseRotation = XMMatrixInverse(nullptr, m_transformReference->getRotationMatrix());
		float magnitude = XMVectorGetX(XMVector3Length(force)) / XMVectorGetX(XMVector3Length(relativePosition));
		m_netTorque += XMVector3Normalize(XMVector3Transform(XMVector3Cross(XMVector3Normalize(relativePosition), XMVector3Normalize(force)), inverseRotation)) * magnitude;
	}

	//XMMATRIX inverseRotation = XMMatrixInverse(nullptr, transformReference->GetRotationMatrix());
	//netTorque += XMVector3Transform(XMVector3Cross(relativePosition, force), inverseRotation);

	void RigidBody::addForceSplit(XMVECTOR position, XMVECTOR force)
	{
		XMVECTOR relativePosition = position - m_transformReference->getPositionVector();

		XMVECTOR toCentreOfMass = XMVector3Normalize(-relativePosition);
		XMVECTOR forceDirection = XMVector3Normalize(force);

		float dotProduct = std::max(XMVectorGetX(XMVector3Dot(toCentreOfMass, forceDirection)), 0.0f);

		addForce(force);// * dotProduct
		addTorque(relativePosition, force * (1.0f - dotProduct));
	}

	void RigidBody::computeAngularAcceleration()
	{
		m_angularAcceleration = XMVector3Transform(m_netTorque, m_inverseInertiaTensor);
	}

	void RigidBody::computeAngluarVelocity(float deltaTime)
	{
		m_angularVelocity += m_angularAcceleration * deltaTime; //v = u + at
	}

	void RigidBody::computeOrientation(float deltaTime)
	{
		XMVECTOR orientationChange = m_angularVelocity * deltaTime;
		XMVECTOR oldOrientation = m_transformReference->getOrientation();
		XMVECTOR newOrientation = XMQuaternionNormalize(XMQuaternionMultiply(orientationChange, oldOrientation));

		m_transformReference->setOrientationQuaternion(newOrientation);
	}

	void RigidBody::applyAngularDrag(float deltaTime)
	{
		m_angularVelocity *= 1.0f - (m_angularDrag * deltaTime);
	}

	void RigidBody::computeSphereIntertiaTensor(float radius)
	{
		// Initialize to identity matrix
		m_inertiaTensor = XMMatrixIdentity();

		// Set matrix values
		m_inertiaTensor.r[0].m128_f32[0] = 0.4f * m_mass * pow(radius, 2);
		m_inertiaTensor.r[1].m128_f32[1] = 0.4f * m_mass * pow(radius, 2);
		m_inertiaTensor.r[2].m128_f32[2] = 0.4f * m_mass * pow(radius, 2);

		// Compute inverse inertia tensor
		XMVECTOR determinant = XMMatrixDeterminant(m_inertiaTensor);
		m_inverseInertiaTensor = XMMatrixInverse(&determinant, m_inertiaTensor);
	}

	void RigidBody::computeShellIntertiaTensor(float radius)
	{
		// Initialize to identity matrix
		m_inertiaTensor = XMMatrixIdentity();

		// Set matrix values
		m_inertiaTensor.r[0].m128_f32[0] = 0.666666f * m_mass * pow(radius, 2);
		m_inertiaTensor.r[1].m128_f32[1] = 0.666666f * m_mass * pow(radius, 2);
		m_inertiaTensor.r[2].m128_f32[2] = 0.666666f * m_mass * pow(radius, 2);

		// Compute inverse inertia tensor
		XMVECTOR determinant = XMMatrixDeterminant(m_inertiaTensor);
		m_inverseInertiaTensor = XMMatrixInverse(&determinant, m_inertiaTensor);
	}

	void RigidBody::computeBoxIntertiaTensor(float sizeX, float sizeY, float sizeZ)
	{
		// Initialize to identity matrix
		m_inertiaTensor = XMMatrixIdentity();

		// Square box bounds
		float sizeXsquared = pow(sizeX, 2);
		float sizeYsquared = pow(sizeY, 2);
		float sizeZsquared = pow(sizeZ, 2);

		// Set matrix values
		m_inertiaTensor.r[0].m128_f32[0] = 0.083333f * m_mass * (sizeYsquared + sizeZsquared);
		m_inertiaTensor.r[1].m128_f32[1] = 0.083333f * m_mass * (sizeXsquared + sizeZsquared);
		m_inertiaTensor.r[2].m128_f32[2] = 0.083333f * m_mass * (sizeXsquared + sizeYsquared);

		// Compute inverse inertia tensor
		XMVECTOR determinant = XMMatrixDeterminant(m_inertiaTensor);
		m_inverseInertiaTensor = XMMatrixInverse(&determinant, m_inertiaTensor);
	}

	XMVECTOR RigidBody::getForceAtRelativePosition(XMVECTOR relativePosition)
	{
		XMMATRIX inverseRotation = XMMatrixInverse(nullptr, m_transformReference->getRotationMatrix());
		XMVECTOR trueRelativePosition = XMVector3Transform(relativePosition, inverseRotation);
		float magnitude = XMVectorGetX(XMVector3Length(relativePosition)) * XMVectorGetX(XMVector3Length(m_angularVelocity));
		XMVECTOR forceLocal = XMVector3Normalize(XMVector3Cross(XMVector3Transform(m_angularVelocity, m_inertiaTensor), relativePosition)) * magnitude;
		return XMVector3Transform(forceLocal, m_transformReference->getRotationMatrix()) + m_mass * getVelocityVector();
	}
}
