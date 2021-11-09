#include "transform.h"

#include "../graphics/imgui/imgui.h"

namespace hrzn::physics
{
	Transform::Transform() :
		m_position(DEFAULT_POSITION),

		m_orientation(IDENDITY_QUATERNION_FLOAT4),
		m_rotationMatrix(IDENTITY_MATRIX_FLOAT4),

		m_up(DEFAULT_UP_FLOAT3),
		m_front(DEFAULT_FRONT_FLOAT3),
		m_right(DEFAULT_RIGHT_FLOAT3)
	{
	}

	Transform::Transform(const XMVECTOR& position, const XMVECTOR& orientationQuaternion)
	{
		setPosition(position);
		setOrientationQuaternion(orientationQuaternion);
	}

	Transform::Transform(const XMFLOAT3& position, const XMVECTOR& orientationQuaternion)
	{
		setPosition(position);
		setOrientationQuaternion(orientationQuaternion);
	}

	Transform::Transform(const XMVECTOR& position, const XMFLOAT4& orientationQuaternion)
	{
		setPosition(position);
		setOrientationQuaternion(orientationQuaternion);
	}

	Transform::Transform(const XMFLOAT3& position, const XMFLOAT4& orientationQuaternion)
	{
		setPosition(position);
		setOrientationQuaternion(orientationQuaternion);
	}

	Transform::Transform(const Transform& transform)
	{
		copyPositionFrom(transform);
		copyOrientationFrom(transform);
	}

	void Transform::copyOrientationFrom(const Transform& otherTransform)
	{
		setOrientationQuaternion(otherTransform.getOrientation());
	}

	void Transform::copyPositionFrom(const Transform& otherTransform)
	{
		setPosition(otherTransform.getPositionVector());
	}

	void Transform::setPosition(const XMVECTOR& position)
	{
		XMStoreFloat3(&m_position, position);
	}

	void Transform::setPosition(const XMFLOAT3& position)
	{
		m_position = position;
	}

	void Transform::setPosition(float x, float y, float z)
	{
		m_position = XMFLOAT3(x, y, z);
	}

	void Transform::adjustPosition(const XMVECTOR& difference)
	{
		setPosition(getPositionVector() + difference);
	}

	void Transform::adjustPosition(const XMFLOAT3& difference)
	{
		adjustPosition(XMLoadFloat3(&difference));
	}

	void Transform::adjustPosition(float diffX, float diffY, float diffZ)
	{
		adjustPosition(XMVectorSet(diffX, diffY, diffZ, 0.0f));
	}

	void Transform::setOrientationQuaternion(const XMVECTOR& orientation, bool updateAll)
	{
		XMStoreFloat4(&m_orientation, XMQuaternionNormalize(orientation));
		if (updateAll)
		{
			XMMATRIX rotationMatrix = XMMatrixRotationQuaternion(orientation);
			XMStoreFloat4x4(&m_rotationMatrix, rotationMatrix);
			updateAxisVectorsUsingRotationMatrix(rotationMatrix);
		}
	}

	void Transform::setOrientationQuaternion(const XMFLOAT4& quaternion, bool updateAll)
	{
		setOrientationQuaternion(XMLoadFloat4(&quaternion), updateAll);
	}

	void Transform::setOrientationRotationMatrix(const XMMATRIX& rotationMatrix, bool updateAll)
	{
		XMStoreFloat4x4(&m_rotationMatrix, rotationMatrix);
		if (updateAll)
		{
			XMStoreFloat4(&m_orientation, XMQuaternionRotationMatrix(rotationMatrix));
			updateAxisVectorsUsingRotationMatrix(rotationMatrix);
		}
	}

	void Transform::setOrientationRollPitchYaw(float roll, float pitch, float yaw)
	{
		setOrientationRotationMatrix(XMMatrixRotationRollPitchYaw(pitch, yaw, roll), true);
	}

	void Transform::lookAtPosition(const XMVECTOR& lookAtPos)
	{
		XMVECTOR front = XMVector3Normalize(lookAtPos - getPositionVector());
		XMVECTOR right = XMVector3Normalize(XMVector3Cross(XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f), front));
		XMVECTOR up;

		if (XMVector3Equal(right, XMVectorZero()))
		{
			right = XMVectorSet(1.0f, 0.0f, 0.0f, 0.0f);
			up = XMVectorSet(0.0f, 0.0f, 1.0f, 0.0f);
		}
		else
		{
			up = XMVector3Normalize(XMVector3Cross(front, right));
		}

		XMStoreFloat3(&m_front, front);
		XMStoreFloat3(&m_right, right);
		XMStoreFloat3(&m_up, up);

		XMMATRIX rotationMatrix = XMMATRIX(right, up, front, XMVectorSet(0.0f, 0.0f, 0.0f, 1.0f));
		rotationMatrix.r[0].m128_f32[3] = 0.0f;
		rotationMatrix.r[1].m128_f32[3] = 0.0f;
		rotationMatrix.r[2].m128_f32[3] = 0.0f;
		//rotationMatrix.r[3].m128_f32[3] = 1.0f;

		XMStoreFloat4x4(&m_rotationMatrix, rotationMatrix);

		XMStoreFloat4(&m_orientation, XMQuaternionRotationMatrix(rotationMatrix));
	}

	void Transform::lookAtPosition(const XMFLOAT3& position)
	{
		lookAtPosition(XMLoadFloat3(&position));
	}

	void Transform::rotateUsingAxis(const XMVECTOR& axis, float angle)
	{
		XMMATRIX rotationMatrix = XMLoadFloat4x4(&m_rotationMatrix);

		rotationMatrix *= XMMatrixRotationAxis(axis, angle);

		setOrientationRotationMatrix(rotationMatrix);
	}

	XMVECTOR Transform::getPositionVector() const
	{
		return XMLoadFloat3(&m_position);
	}

	const XMFLOAT3& Transform::getPositionFloat3() const
	{
		return m_position;
	}

	XMVECTOR Transform::getOrientation() const
	{
		return XMLoadFloat4(&m_orientation);
	}

	const XMFLOAT4& Transform::getOrientationFloat4() const
	{
		return m_orientation;
	}

	XMMATRIX Transform::getRotationMatrix() const
	{
		return XMLoadFloat4x4(&m_rotationMatrix);
	}

	XMVECTOR Transform::getFrontVector() const
	{
		return XMLoadFloat3(&m_front);
	}

	const XMFLOAT3& hrzn::physics::Transform::getFrontFloat3() const
	{
		return m_front;
	}

	XMVECTOR Transform::getBackVector() const
	{
		return -getFrontVector();
	}

	const XMFLOAT3& hrzn::physics::Transform::getBackFloat3() const
	{
		return XMFLOAT3(-m_front.x, -m_front.y, -m_front.z);
	}

	XMVECTOR Transform::getUpVector() const
	{
		return XMLoadFloat3(&m_up);
	}

	const XMFLOAT3& hrzn::physics::Transform::getUpFloat3() const
	{
		return m_up;
	}

	XMVECTOR Transform::getDownVector() const
	{
		return -getUpVector();
	}

	const XMFLOAT3& hrzn::physics::Transform::getDownFloat3() const
	{
		return XMFLOAT3(-m_up.x, -m_up.y, -m_up.z);
	}

	XMVECTOR Transform::getRightVector() const
	{
		return XMLoadFloat3(&m_right);
	}

	const XMFLOAT3& hrzn::physics::Transform::getRightFloat3() const
	{
		return m_right;
	}

	XMVECTOR Transform::getLeftVector() const
	{
		return -getRightVector();
	}

	const XMFLOAT3& hrzn::physics::Transform::getLeftFloat3() const
	{
		return XMFLOAT3(-m_right.x, -m_right.y, -m_right.z);
	}

	void Transform::updateAxisVectorsUsingRotationMatrix(const XMMATRIX& rotationMatrix)
	{
		XMStoreFloat3(&m_front, XMVector3Transform(DEFAULT_FRONT_VECTOR, rotationMatrix));
		XMStoreFloat3(&m_right, XMVector3Transform(DEFAULT_RIGHT_VECTOR, rotationMatrix));
		XMStoreFloat3(&m_up, XMVector3Transform(DEFAULT_UP_VECTOR, rotationMatrix));
	}

	void Transform::editPositionImGui()
	{
		ImGui::DragFloat3("Position", &m_position.x, 0.2f, -500.0f, 500.0f);
	}

	void Transform::displayOrientationImGui() const
	{
		ImGui::Text("quat -> (%f, %f, %f, %f)", m_orientation.x, m_orientation.y, m_orientation.z, m_orientation.w);
		ImGui::Text("front -> (%f, %f, %f)", m_front.x, m_front.y, m_front.z);
		ImGui::Text("up -> (%f, %f, %f)", m_up.x, m_up.y, m_up.z);
		ImGui::Text("right -> (%f, %f, %f)", m_right.x, m_right.y, m_right.z);
	}
}
