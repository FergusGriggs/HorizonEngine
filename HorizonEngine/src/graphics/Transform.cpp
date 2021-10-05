#include "Transform.h"

Transform::Transform()
{
}

Transform::Transform(const XMVECTOR& position, const XMVECTOR& orientationQuaternion)
{
	this->SetPosition(position);
	this->SetOrientationQuaternion(orientationQuaternion);
}

Transform::Transform(const Transform& transform)
{
	this->CopyPositionFrom(transform);
	this->CopyOrientationFrom(transform);
}

void Transform::CopyOrientationFrom(const Transform& otherTransform)
{
	this->SetOrientationQuaternion(otherTransform.GetOrientation());
}

void Transform::CopyPositionFrom(const Transform& otherTransform)
{
	this->SetPosition(otherTransform.GetPositionVector());
}

void Transform::SetPosition(const XMVECTOR& position)
{
	this->positionVector = position;
	XMStoreFloat3(&this->positionFloat3, this->positionVector);
}

void Transform::SetPosition(const XMFLOAT3& position)
{
	this->positionFloat3 = position;
	this->positionVector = XMLoadFloat3(&this->positionFloat3);
}

void Transform::SetPosition(float x, float y, float z)
{
	this->positionVector = XMVectorSet(x, y, z, 1.0f);
	XMStoreFloat3(&this->positionFloat3, this->positionVector);
}

void Transform::AdjustPosition(const XMVECTOR& difference)
{
	this->positionVector += difference;
	XMStoreFloat3(&this->positionFloat3, this->positionVector);
}

void Transform::AdjustPosition(const XMFLOAT3& difference)
{
	this->positionFloat3.x += difference.x;
	this->positionFloat3.y += difference.y;
	this->positionFloat3.z += difference.z;

	this->positionVector = XMLoadFloat3(&this->positionFloat3);
}

void Transform::AdjustPosition(float diffX, float diffY, float diffZ)
{
	this->positionVector += XMVectorSet(diffX, diffY, diffZ, 0.0f);
	XMStoreFloat3(&this->positionFloat3, this->positionVector);
}

void Transform::SetOrientationQuaternion(const XMVECTOR& orientation, bool updateAll)
{
	this->orientation = orientation;
	if (updateAll)
	{
		this->rotationMatrix = XMMatrixRotationQuaternion(this->orientation);
		this->UpdateAxisVectorsUsingRotationMatrix();
	}
}

void Transform::SetOrientationRotationMatrix(const XMMATRIX& rotationMatrix, bool updateAll)
{
	this->rotationMatrix = rotationMatrix;
	if (updateAll)
	{
		this->orientation = XMQuaternionRotationMatrix(this->rotationMatrix);
		this->UpdateAxisVectorsUsingRotationMatrix();
	}
}

void Transform::SetOrientationRollPitchYaw(float roll, float pitch, float yaw)
{
	this->rotationMatrix = XMMatrixRotationRollPitchYaw(pitch, yaw, roll);
	this->orientation = XMQuaternionRotationMatrix(this->rotationMatrix);
	this->UpdateAxisVectorsUsingRotationMatrix();
}

void Transform::LookAtPos(XMVECTOR lookAtPos)
{
	this->front = XMVector3Normalize(lookAtPos - this->positionVector);
	this->right = XMVector3Normalize(XMVector3Cross(XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f), this->front));
	if (XMVector3Equal(this->right, XMVectorZero())) {
		this->right = XMVectorSet(1.0f, 0.0f, 0.0f, 0.0f);
		this->up = XMVectorSet(0.0f, 0.0f, 1.0f, 0.0f);
	}
	else {
		this->up = XMVector3Normalize(XMVector3Cross(this->front, this->right));
	}

	this->rotationMatrix = XMMATRIX(this->right, this->up, this->front, XMVectorSet(0.0f, 0.0f, 0.0f, 1.0f));

	this->rotationMatrix.r[0].m128_f32[3] = 0.0f;
	this->rotationMatrix.r[1].m128_f32[3] = 0.0f;
	this->rotationMatrix.r[2].m128_f32[3] = 0.0f;

	this->orientation = XMQuaternionRotationMatrix(this->rotationMatrix);
}

void Transform::LookAtPos(XMFLOAT3 lookAtPos)
{
	XMVECTOR lookAtPosVector = XMLoadFloat3(&lookAtPos);
	this->LookAtPos(lookAtPosVector);
}

void Transform::RotateUsingAxis(XMVECTOR axis, float angle)
{
	this->rotationMatrix *= XMMatrixRotationAxis(axis, angle);
	this->orientation = XMQuaternionRotationMatrix(this->rotationMatrix);

	//this->quaternion *= XMQuaternionRotationAxis(axis, angle);
	//this->rotationMatrix = XMMatrixRotationQuaternion(this->quaternion);

	this->UpdateAxisVectorsUsingRotationMatrix();
}

const XMVECTOR& Transform::GetOrientation() const
{
	return this->orientation;
}

const XMFLOAT4& Transform::GetOrientationFloat4() const
{
	XMFLOAT4 returnFloat4;
	XMStoreFloat4(&returnFloat4, this->orientation);
	return returnFloat4;
}

const XMMATRIX& Transform::GetRotationMatrix() const
{
	return this->rotationMatrix;
}

const XMVECTOR& Transform::GetFrontVector() const
{
	return this->front;
}

const XMVECTOR& Transform::GetBackVector() const
{
	return -this->front;
}

const XMVECTOR& Transform::GetUpVector() const
{
	return this->up;
}

const XMVECTOR& Transform::GetDownVector() const
{
	return -this->up;
}

const XMVECTOR& Transform::GetRightVector() const
{
	return this->right;
}

const XMVECTOR& Transform::GetLeftVector() const
{
	return -this->right;
}

void Transform::UpdateAxisVectorsUsingRotationMatrix()
{
	this->front = XMVector3Transform(DEFAULT_FRONT_VECTOR, this->rotationMatrix);
	this->right = XMVector3Transform(DEFAULT_RIGHT_VECTOR, this->rotationMatrix);
	this->up = XMVector3Transform(DEFAULT_UP_VECTOR, this->rotationMatrix);
}

const XMVECTOR& Transform::GetPositionVector() const
{
	return this->positionVector;
}

const XMFLOAT3& Transform::GetPositionFloat3() const
{
	return this->positionFloat3;
}