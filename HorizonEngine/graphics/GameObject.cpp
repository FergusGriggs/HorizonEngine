#include "GameObject.h"

GameObject::GameObject()
{
	this->type = GameObjectType::BASE;
}

const XMVECTOR& GameObject::GetPositionVector() const
{
	return this->positionVector;
}

const XMFLOAT3& GameObject::GetPositionFloat3() const
{
	return this->position;
}

void GameObject::SetPosition(const XMVECTOR& position)
{
	XMStoreFloat3(&this->position, position);
	this->positionVector = position;
	this->UpdateModelMatrix();
}

void GameObject::SetPosition(const XMFLOAT3& position)
{
	this->position = position;
	this->positionVector = XMLoadFloat3(&this->position);
	this->UpdateModelMatrix();
}


void GameObject::SetPosition(float x, float y, float z)
{
	this->position = XMFLOAT3(x, y, z);
	this->positionVector = XMLoadFloat3(&this->position);
	this->UpdateModelMatrix();
}

void GameObject::AdjustPosition(const XMVECTOR& diff)
{
	this->positionVector += diff;
	XMStoreFloat3(&this->position, this->positionVector);
	this->UpdateModelMatrix();
}

void GameObject::AdjustPosition(const XMFLOAT3& diff)
{
	this->position.x += diff.x;
	this->position.y += diff.y;
	this->position.z += diff.z;
	this->positionVector = XMLoadFloat3(&this->position);
	this->UpdateModelMatrix();
}

void GameObject::AdjustPosition(float x, float y, float z)
{
	this->position.x += x;
	this->position.y += y;
	this->position.z += z;
	this->positionVector = XMLoadFloat3(&this->position);
	this->UpdateModelMatrix();
}

void GameObject::SetRotation(const XMVECTOR& rotation)
{
	this->rotationMatrix = XMMatrixRotationRollPitchYaw(XMVectorGetX(rotation), XMVectorGetY(rotation), XMVectorGetZ(rotation));
	this->CreateAxisVectorsFromRotMat();
	this->UpdateModelMatrix();
}

void GameObject::SetRotation(const XMFLOAT3& rotation)
{
	this->rotationMatrix = XMMatrixRotationRollPitchYaw(rotation.x, rotation.y, rotation.z);
	this->CreateAxisVectorsFromRotMat();
	this->UpdateModelMatrix();
}

void GameObject::SetRotation(float x, float y, float z)
{
	this->rotationMatrix = XMMatrixRotationRollPitchYaw(x, y, z);
	this->CreateAxisVectorsFromRotMat();
	this->UpdateModelMatrix();
}

void GameObject::UpdateModelMatrix()
{
	this->modelMatrix = this->rotationMatrix * XMMatrixTranslation(this->position.x, this->position.y, this->position.z);
}

void GameObject::RotateAxisVectors(XMVECTOR axis, float angle)
{
	XMMATRIX rotation = XMMatrixRotationAxis(axis, angle);

	this->front = XMVector3Transform(this->front, rotation);
	this->right = XMVector3Transform(this->right, rotation);
	this->up = XMVector3Transform(this->up, rotation);

	this->rotationMatrix = XMMatrixLookToLH(XMVectorSet(0.0f, 0.0f, 0.0f, 0.0f), this->front, this->up);
	//this->rotationMatrix = 
	//this->rotationMatrix = XMMATRIX(front, right, up, XMVectorSet(0.0f, 0.0f, 0.0f, 1.0f));
	//this->rotationMatrix = XMMATRIX(-right, up, front, XMVectorSet(0.0f, 0.0f, 0.0f, 1.0f));
	//this->rotationMatrix = XMMatrixTranspose(XMMATRIX(front, up, right, XMVectorSet(0.0f, 0.0f, 0.0f, 1.0f)));

	UpdateModelMatrix();
}

void GameObject::CopyAxisVectorsFrom(GameObject* gameObject)
{
	this->front = gameObject->front;
	this->right = gameObject->right;
	this->up = gameObject->up;
}

void GameObject::SetLookAtPos(XMFLOAT3 lookAtPos)
{
	this->front = XMVector3Normalize(XMVectorSet(lookAtPos.x, lookAtPos.y, lookAtPos.z, 0.0f) - this->positionVector);
	this->right = XMVector3Normalize(XMVector3Cross(this->front, XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f)));
	if (XMVector3Equal(right, XMVectorZero())) {
		this->right = XMVectorSet(-1.0f, 0.0f, 0.0f, 0.0f);
		this->up = XMVectorSet(0.0f, 0.0f, 1.0f, 0.0f);
	}
	else {
		this->up = XMVector3Normalize(XMVector3Cross(this->right, this->front));
	}

	this->rotationMatrix = XMMATRIX(-right, up, front, XMVectorSet(0.0f, 0.0f, 0.0f, 1.0f));

	/*if (lookAtPos.x == this->position.x && lookAtPos.y == this->position.y && lookAtPos.z == this->position.z) {
		return;
	}

	lookAtPos.x = this->position.x - lookAtPos.x;
	lookAtPos.y = this->position.y - lookAtPos.y;
	lookAtPos.z = this->position.z - lookAtPos.z;

	float pitch = 0.0f;
	if (lookAtPos.y != 0.0f) {
		const float distance = sqrt(pow(lookAtPos.x, 2) + pow(lookAtPos.z, 2));
		pitch = atan(lookAtPos.y / distance);
	}

	float yaw = 0.0f;
	if (lookAtPos.x != 0.0f) {
		yaw = atan(lookAtPos.x / lookAtPos.z);
	}

	if (lookAtPos.z > 0) {
		yaw += XM_PI;
	}

	this->SetRotation(pitch, yaw, 0.0f);*/
}

const XMVECTOR& GameObject::GetFrontVector(bool noY)
{
	if (noY)
		return this->frontNoY;
	return this->front;
}

const XMVECTOR& GameObject::GetBackVector(bool noY)
{
	if (noY)
		return -this->frontNoY;
	return -this->front;
}

const XMVECTOR& GameObject::GetLeftVector(bool noY)
{
	if (noY)
		return -this->rightNoY;
	return -this->right;
}

const XMVECTOR& GameObject::GetRightVector(bool noY)
{
	if (noY)
		return this->rightNoY;
	return this->right;
}

const XMVECTOR& GameObject::GetUpVector()
{
	return this->up;
}

void GameObject::Update(float deltaTime)
{
	if (followingTrack) {
		this->objectTrack->Follow(this, deltaTime);
	}
}

GameObjectType GameObject::GetType()
{
	return this->type;
}

std::string GameObject::GetLabel()
{
	return this->label;
}

XMMATRIX GameObject::GetModelMatrix()
{
	return this->modelMatrix;
}

XMMATRIX GameObject::GetRotationMatrix()
{
	return this->rotationMatrix;
}


void GameObject::SetObjectDelta(float delta)
{
	assert("SetDelta() must be overridden." && 0);
}


void GameObject::CreateAxisVectorsFromRotMat()
{
	this->front = XMVector3Transform(this->DEFAULT_FRONT_VECTOR, this->rotationMatrix);
	this->right = XMVector3Transform(this->DEFAULT_RIGHT_VECTOR, this->rotationMatrix);
	this->up = XMVector3Transform(this->DEFAULT_UP_VECTOR, this->rotationMatrix);
}

float GameObject::GetObjectTrackDelta()
{
	return this->objectTrack->GetDelta();

}
void GameObject::SetObjectTrackDelta(float objectTrackDelta)
{
	this->objectTrack->SetDelta(objectTrackDelta);
}

bool GameObject::GetFollowingObjectTrack()
{
	return followingTrack;
}

void GameObject::SetFollowingObjectTrack(bool followingTrack)
{
	this->followingTrack = followingTrack;
}

void GameObject::SetObjectTrack(ObjectTrack* objectTrack) {
	this->objectTrack = objectTrack;
}

ObjectTrack* GameObject::GetObjectTrack() {
	return this->objectTrack;
}