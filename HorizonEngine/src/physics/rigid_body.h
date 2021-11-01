#pragma once

#include "particle_model.h"

#include <algorithm>

namespace hrzn::physics
{
	class RigidBody : public ParticleModel
	{
	public:
		RigidBody();

		void update(float deltaTime) override;

		void addTorque(XMVECTOR relativePosition, XMVECTOR force);
		void addForceSplit(XMVECTOR position, XMVECTOR force);

		void computeAngularAcceleration();
		void computeAngluarVelocity(float deltaTime);
		void computeOrientation(float deltaTime);

		void applyAngularDrag(float deltaTime);

		void computeSphereIntertiaTensor(float radius);
		void computeShellIntertiaTensor(float radius);
		void computeBoxIntertiaTensor(float sizeX, float sizeY, float sizeZ);

		XMVECTOR getForceAtRelativePosition(XMVECTOR relativePosition);

	private:
		XMVECTOR m_netTorque;
		XMVECTOR m_angularAcceleration;
		XMVECTOR m_angularVelocity;

		XMMATRIX m_inertiaTensor;
		XMMATRIX m_inverseInertiaTensor;

		float    m_angularDrag;
	};
}
