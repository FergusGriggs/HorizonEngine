#pragma once

#include <vector>

#include <DirectXMath.h>

#include "transform.h"

using namespace DirectX;

namespace hrzn::physics
{
	const float GRAVITY = -9.81f;

	enum class ParticleModelType
	{
		eBase,
		eRigidBody,
	};

	class ParticleModel
	{
	public:
		ParticleModel();

		ParticleModelType getType();

		void setIsStatic(bool isStatic);
		void setMass(float mass);
		void setDrag(float drag);

		bool  isStatic();
		bool* isStaticPtr();
		float getMass();
		float getDrag();

		void  computeAcceleration(float deltaTime);
		void  computeVelocity(float deltaTime);
		void  computePosition(float deltaTime);
		void  applyThrustForces(float deltaTime);

		void  addThrust(const XMVECTOR& force, float duration);
		void  addThrust(const XMFLOAT3& force, float duration);
		void  addForce(const XMVECTOR& force);
		void  addForce(const XMFLOAT3& force);

		virtual void update(float deltaTime);

		void applyGravity(float deltaTime);
		void applyDrag(float deltaTime);
		void applyDragTurbulentFlow();
		void applyDragLaminarFlow();

		void       setTransformReference(Transform* transformReference);
		Transform* getTransformReference();

		void       checkFloorHeight();

		void            setVelocity(const XMVECTOR& velocity);
		void            setVelocity(const XMFLOAT3& velocity);

		const XMVECTOR& getVelocityVector();
		const XMFLOAT3& getVelocityFloat3();

		const XMVECTOR& getAccelerationVector();
		const XMFLOAT3& getAccelerationFloat3();

	protected:
		ParticleModelType m_type;

		Transform* m_transformReference;

		XMFLOAT3 m_velocity;
		XMFLOAT3 m_acceleration;
		XMFLOAT3 m_netForce;

		std::vector<std::pair<XMFLOAT3, float>> m_thrustForces;

		bool  m_isStatic;

		float m_mass;
		float m_drag;
		float m_floorHeight;
	};
}
