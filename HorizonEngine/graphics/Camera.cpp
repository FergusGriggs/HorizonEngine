#include "Camera.h"

Camera::Camera()
{
	this->position = XMFLOAT3(0.0f, 0.0f, 0.0f);
	this->positionVector = XMLoadFloat3(&this->position);

	this->rotation = XMFLOAT3(0.0f, 0.0f, 0.0f);
	this->rotationVector = XMLoadFloat3(&this->rotation);

	UpdateMatrix();

	this->type = GameObjectType::CAMERA;
}

void Camera::Update(float deltaTime)
{
	if (followingTrack) {
		this->objectTrack->Follow(this, deltaTime);
	}
}


void Camera::SetProjectionValues(float fovDegrees, float aspectRatio, float nearZ, float farZ)
{
	this->fov = fovDegrees;
	this->aspectRatio = aspectRatio;
	this->nearZ = nearZ;
	this->farZ = farZ;
	float fovRadians = (fovDegrees / 180.0f) * XM_PI;
	this->projection = XMMatrixPerspectiveFovLH(fovRadians, aspectRatio, nearZ, farZ);
}

void Camera::Zoom(float fovDiff)
{
	this->fov += fovDiff;
	if (this->fov < 10.0f) {
		this->fov = 10.0f;
	}
	else if (this->fov > 150.0f) {
		this->fov = 150.0f;
	}
	float fovRadians = (this->fov / 180.0f) * XM_PI;
	this->projection = XMMatrixPerspectiveFovLH(fovRadians, this->aspectRatio, this->nearZ, this->farZ);
}

float Camera::GetZoom()
{
	return this->fov;
}

const XMMATRIX& Camera::GetViewMatrix() const
{
	return this->view;
}

const XMMATRIX& Camera::GetProjectionMatrix() const
{
	return this->projection;
}

void Camera::UpdateMatrix()
{
	XMMATRIX cameraRotationMatrix = XMMatrixRotationRollPitchYaw(this->rotation.x, this->rotation.y, this->rotation.z);
	XMVECTOR cameraTarget = XMVector3TransformCoord(this->DEFAULT_FRONT_VECTOR, cameraRotationMatrix);
	cameraTarget += this->positionVector;
	XMVECTOR cameraUp = XMVector3TransformCoord(this->DEFAULT_UP_VECTOR, cameraRotationMatrix);
	this->view = XMMatrixLookAtLH(this->positionVector, cameraTarget, cameraUp);

	this->UpdateDirectionVectors();
}

void Camera::ComputeMouseToWorldVectorDirection(float mouseNDCX, float mouseNDCY)
{
	XMMATRIX inverseView = XMMatrixInverse(nullptr, view);
	XMMATRIX inverseProjection = XMMatrixInverse(nullptr, projection);

	XMVECTOR clipSpaceVector = XMVectorSet(mouseNDCX, mouseNDCY, 0.0f, 0.0f);

	XMVECTOR viewSpaceVector = XMVector3Transform(clipSpaceVector, inverseProjection);

	XMVECTOR worldSpaceVector = XMVector3Transform(viewSpaceVector, inverseView);

	this->mouseToWorldVectorDirection = XMVector3Normalize(worldSpaceVector - positionVector);
}

XMVECTOR Camera::GetMouseToWorldVectorDirection()
{
	return this->mouseToWorldVectorDirection;
}

XMFLOAT2 Camera::GetNDCFrom3DPos(XMVECTOR objectPosition)
{
	XMVECTOR viewSpaceVector = XMVector3Transform(objectPosition, view);

	XMVECTOR clipSpaceVector = XMVector3Transform(viewSpaceVector, projection);

	float w = XMVectorGetW(clipSpaceVector);
	return XMFLOAT2(XMVectorGetX(clipSpaceVector) / w, XMVectorGetY(clipSpaceVector) / w);
}
