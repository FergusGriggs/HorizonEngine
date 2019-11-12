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

const XMVECTOR& GameObject::GetRotationVector() const
{
	return this->rotationVector;
}

const XMFLOAT3& GameObject::GetRotationFloat3() const
{
	return this->rotation;
}

void GameObject::SetPosition(const XMVECTOR& position)
{
	XMStoreFloat3(&this->position, position);
	this->positionVector = position;
	this->UpdateMatrix();
}

void GameObject::SetPosition(const XMFLOAT3& position)
{
	this->position = position;
	this->positionVector = XMLoadFloat3(&this->position);
	this->UpdateMatrix();
}


void GameObject::SetPosition(float x, float y, float z)
{
	this->position = XMFLOAT3(x, y, z);
	this->positionVector = XMLoadFloat3(&this->position);
	this->UpdateMatrix();
}

void GameObject::AdjustPosition(const XMVECTOR& diff)
{
	this->positionVector += diff;
	XMStoreFloat3(&this->position, this->positionVector);
	this->UpdateMatrix();
}

void GameObject::AdjustPosition(const XMFLOAT3& diff)
{
	this->position.x += diff.x;
	this->position.y += diff.y;
	this->position.z += diff.z;
	this->positionVector = XMLoadFloat3(&this->position);
	this->UpdateMatrix();
}

void GameObject::AdjustPosition(float x, float y, float z)
{
	this->position.x += x;
	this->position.y += y;
	this->position.z += z;
	this->positionVector = XMLoadFloat3(&this->position);
	this->UpdateMatrix();
}

void GameObject::SetRotation(const XMVECTOR& rotation)
{
	XMStoreFloat3(&this->rotation, rotation);
	this->rotationVector = rotation;
	this->UpdateMatrix();
}

void GameObject::SetRotation(const XMFLOAT3& rotation)
{
	this->rotation = rotation;
	this->rotationVector = XMLoadFloat3(&this->rotation);
	this->UpdateMatrix();
}

void GameObject::SetRotation(float x, float y, float z)
{
	this->rotation = XMFLOAT3(x, y, z);
	this->rotationVector = XMLoadFloat3(&this->rotation);
	this->UpdateMatrix();
}

void GameObject::AdjustRotation(const XMVECTOR& rotDiff)
{
	this->rotationVector += rotDiff;
	XMStoreFloat3(&this->rotation, this->rotationVector);
	this->UpdateMatrix();
}

void GameObject::AdjustRotation(const XMFLOAT3& rotDiff)
{
	this->rotation.x += rotation.x;
	this->rotation.y += rotation.y;
	this->rotation.z += rotation.z;
	this->rotationVector = XMLoadFloat3(&this->rotation);
	this->UpdateMatrix();
}

void GameObject::AdjustRotation(float x, float y, float z)
{
	this->rotation.x += x;
	this->rotation.y += y;
	this->rotation.z += z;
	this->rotationVector = XMLoadFloat3(&this->rotation);
	this->UpdateMatrix();
}

void GameObject::SetLookAtPos(XMFLOAT3 lookAtPos)
{
	if (lookAtPos.x == this->position.x && lookAtPos.y == this->position.y && lookAtPos.z == this->position.z) {
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

	this->SetRotation(pitch, yaw, 0.0f);
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
		return this->backNoY;
	return this->back;
}

const XMVECTOR& GameObject::GetLeftVector(bool noY)
{
	if (noY)
		return this->leftNoY;
	return this->left;
}

const XMVECTOR& GameObject::GetRightVector(bool noY)
{
	if (noY)
		return this->rightNoY;
	return this->right;
}


void GameObject::UpdateMatrix()
{
	assert("UpdateMatrix() must be overridden." && 0);
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

void GameObject::SetObjectDelta(float delta)
{
	assert("SetDelta() must be overridden." && 0);
}


void GameObject::UpdateDirectionVectors()
{
	XMMATRIX vectorRotationMatrix = XMMatrixRotationRollPitchYaw(this->rotation.x, this->rotation.y, 0.0f);
	this->front = XMVector3TransformCoord(this->DEFAULT_FRONT_VECTOR, vectorRotationMatrix);
	this->back = XMVector3TransformCoord(this->DEFAULT_BACK_VECTOR, vectorRotationMatrix);
	this->left = XMVector3TransformCoord(this->DEFAULT_LEFT_VECTOR, vectorRotationMatrix);
	this->right = XMVector3TransformCoord(this->DEFAULT_RIGHT_VECTOR, vectorRotationMatrix);

	XMMATRIX vectorRotationMatrixNoY = XMMatrixRotationRollPitchYaw(0.0f, this->rotation.y, 0.0f);
	this->frontNoY = XMVector3TransformCoord(this->DEFAULT_FRONT_VECTOR, vectorRotationMatrixNoY);
	this->backNoY = XMVector3TransformCoord(this->DEFAULT_BACK_VECTOR, vectorRotationMatrixNoY);
	this->leftNoY = XMVector3TransformCoord(this->DEFAULT_LEFT_VECTOR, vectorRotationMatrixNoY);
	this->rightNoY = XMVector3TransformCoord(this->DEFAULT_RIGHT_VECTOR, vectorRotationMatrixNoY);
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