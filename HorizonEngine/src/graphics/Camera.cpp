//Camera.cpp
//Function implementations for the Camera class

#include "Camera.h"

Camera::Camera()
{
	this->label = "Camera";

	this->type = GameObjectType::CAMERA;

	this->relativeObject = nullptr;
}

void Camera::Update(float deltaTime)
{
	if (usingRelativePosition && relativeObject != nullptr)
	{
		this->transform.SetPosition(relativeObject->GetTransform()->GetPositionVector() + relativeObject->GetTransform()->GetRightVector() * XMVectorGetX(relativePosition) + relativeObject->GetTransform()->GetUpVector() * XMVectorGetY(relativePosition) + relativeObject->GetTransform()->GetFrontVector() * XMVectorGetZ(relativePosition));
	}
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

void Camera::SetZoom(float fov)
{
	this->fov = fov;
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

void Camera::UpdateView()
{
	this->view = XMMatrixLookToLH(this->transform.GetPositionVector(), this->transform.GetFrontVector(), this->transform.GetUpVector());
}

void Camera::ComputeMouseToWorldVectorDirection(float mouseNDCX, float mouseNDCY)
{
	XMMATRIX inverseView = XMMatrixInverse(nullptr, view);
	XMMATRIX inverseProjection = XMMatrixInverse(nullptr, projection);

	XMVECTOR clipSpaceVector = XMVectorSet(mouseNDCX, mouseNDCY, 0.0f, 0.0f);

	XMVECTOR viewSpaceVector = XMVector3Transform(clipSpaceVector, inverseProjection);

	XMVECTOR worldSpaceVector = XMVector3Transform(viewSpaceVector, inverseView);

	this->mouseToWorldVectorDirection = XMVector3Normalize(worldSpaceVector - this->transform.GetPositionVector());
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

void Camera::SetRelativeObject(GameObject* relativeObject, XMVECTOR relativePosition)
{
	this->relativeObject = relativeObject;
	this->relativePosition = relativePosition;
	this->usingRelativePosition = true;
}

bool* Camera::GetUsingRelativeCameraPtr()
{
	return &usingRelativePosition;
}
