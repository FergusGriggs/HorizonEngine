#pragma once

#include <DirectXMath.h>

using namespace DirectX;

#define DEFAULT_FRONT_VECTOR XMVectorSet(0.0f, 0.0f, 1.0f, 0.0f)
#define DEFAULT_UP_VECTOR XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f)
#define DEFAULT_RIGHT_VECTOR XMVectorSet(1.0f, 0.0f, 0.0f, 0.0f)

#define DEFAULT_FRONT_FLOAT3 XMFLOAT3(0.0f, 0.0f, 1.0f)
#define DEFAULT_UP_FLOAT3 XMFLOAT3(0.0f, 1.0f, 0.0f)
#define DEFAULT_RIGHT_FLOAT3 XMFLOAT3(1.0f, 0.0f, 0.0f)

#define DEFAULT_POSITION XMFLOAT3(0.0f, 0.0f, 0.0f)

#define IDENDITY_QUATERNION_FLOAT4 XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f)
#define IDENTITY_MATRIX_FLOAT4 XMFLOAT4X4(1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f)

namespace hrzn::entity
{
	class Transform
	{
	public:
		//Constructors
		Transform();

		Transform(const XMVECTOR& position, const XMVECTOR& orientationQuaternion);
		Transform(const XMFLOAT3& position, const XMVECTOR& orientationQuaternion);
		Transform(const XMVECTOR& position, const XMFLOAT4& orientationQuaternion);
		Transform(const XMFLOAT3& position, const XMFLOAT4& orientationQuaternion);

		Transform(const Transform& transform);

		// Copy functions
		void copyOrientationFrom(const Transform& otherTransform);
		void copyPositionFrom(const Transform& otherTransform);

		// Position set functions
		void setPosition(const XMVECTOR& position);
		void setPosition(const XMFLOAT3& position);
		void setPosition(float x, float y, float z);

		//Adjust position functions
		void adjustPosition(const XMVECTOR& difference);
		void adjustPosition(const XMFLOAT3& difference);
		void adjustPosition(float diffX, float diffY, float diffZ);

		// Orientation set functions
		void setOrientationQuaternion(const XMVECTOR& quaternion, bool updateAll = true);
		void setOrientationQuaternion(const XMFLOAT4& quaternion, bool updateAll = true);
		void setOrientationRotationMatrix(const XMMATRIX& rotationMatrix, bool updateAll = true);
		void setOrientationRollPitchYaw(float roll, float pitch, float yaw);

		//Other orientation functions
		void lookAtPosition(const XMVECTOR& position);
		void lookAtPosition(const XMFLOAT3& position);
		void rotateUsingAxis(const XMVECTOR& axis, float angle);

		// Position get functions
		const XMVECTOR& getPositionVector() const;
		const XMFLOAT3& getPositionFloat3() const;

		// Orientation get functions
		const XMVECTOR& getOrientation() const;
		const XMFLOAT4& getOrientationFloat4() const;
		const XMMATRIX& getRotationMatrix() const;

		const XMVECTOR& getFrontVector() const;
		const XMVECTOR& getBackVector() const;
		const XMVECTOR& getUpVector() const;
		const XMVECTOR& getDownVector() const;
		const XMVECTOR& getRightVector() const;
		const XMVECTOR& getLeftVector() const;

		void updateAxisVectorsUsingRotationMatrix(const XMMATRIX& rotationMatrix);

		void editPositionImGui();
		void showAxisVectorsImGui() const;

	private:
		// Position info
		XMFLOAT3   m_position;

		// Orientation info
		XMFLOAT4   m_orientation;
		XMFLOAT4X4 m_rotationMatrix;

		XMFLOAT3   m_up;
		XMFLOAT3   m_front;
		XMFLOAT3   m_right;
	};
}
