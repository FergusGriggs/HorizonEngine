//GameObject.cpp
//Function implementations for the GameObject class

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

std::vector<XMVECTOR>* GameObject::GetRelativePositions()
{
	return &relativePositions;
}

void GameObject::RotateAxisVectors(XMVECTOR axis, float angle)
{
	XMMATRIX rotation = XMMatrixRotationAxis(axis, angle);

	this->front = XMVector4Transform(this->front, rotation);
	this->right = XMVector4Transform(this->right, rotation);
	this->up = XMVector4Transform(this->up, rotation);

	this->rotationMatrix = XMMATRIX(right, up, front, XMVectorSet(0.0f, 0.0f, 0.0f, 1.0f));

	this->rotationMatrix.r[0].m128_f32[3] = 0.0f;
	this->rotationMatrix.r[1].m128_f32[3] = 0.0f;
	this->rotationMatrix.r[2].m128_f32[3] = 0.0f;
	//this->rotationMatrix = XMMatrixLookToLH(XMVectorSet(0.0f, 0.0f, 0.0f, 0.0f), this->front, this->up);
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

	this->rotationMatrix = XMMATRIX(this->right, this->up, this->front, XMVectorSet(0.0f, 0.0f, 0.0f, 1.0f));

	this->rotationMatrix.r[0].m128_f32[3] = 0.0f;
	this->rotationMatrix.r[1].m128_f32[3] = 0.0f;
	this->rotationMatrix.r[2].m128_f32[3] = 0.0f;

	UpdateModelMatrix();
}

void GameObject::SetLookAtPos(XMFLOAT3 lookAtPos)
{
	this->front = XMVector3Normalize(XMVectorSet(lookAtPos.x, lookAtPos.y, lookAtPos.z, 0.0f) - this->positionVector);
	this->right = XMVector3Normalize(XMVector3Cross(XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f), this->front));
	if (XMVector3Equal(right, XMVectorZero())) {
		this->right = XMVectorSet(1.0f, 0.0f, 0.0f, 0.0f);
		this->up = XMVectorSet(0.0f, 0.0f, 1.0f, 0.0f);
	}
	else {
		this->up = XMVector3Normalize(XMVector3Cross(this->front, this->right));
	}

	this->rotationMatrix = XMMATRIX(right, up, front, XMVectorSet(0.0f, 0.0f, 0.0f, 1.0f));

	this->rotationMatrix.r[0].m128_f32[3] = 0.0f;
	this->rotationMatrix.r[1].m128_f32[3] = 0.0f;
	this->rotationMatrix.r[2].m128_f32[3] = 0.0f;

	this->UpdateModelMatrix();
}

void GameObject::SetLookAtPos(XMVECTOR lookAtPos)
{
	this->front = XMVector3Normalize(lookAtPos - this->positionVector);
	this->right = XMVector3Normalize(XMVector3Cross(XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f), this->front));
	if (XMVector3Equal(right, XMVectorZero())) {
		this->right = XMVectorSet(1.0f, 0.0f, 0.0f, 0.0f);
		this->up = XMVectorSet(0.0f, 0.0f, 1.0f, 0.0f);
	}
	else {
		this->up = XMVector3Normalize(XMVector3Cross(this->front, this->right));
	}

	this->rotationMatrix = XMMATRIX(right, up, front, XMVectorSet(0.0f, 0.0f, 0.0f, 1.0f));

	this->rotationMatrix.r[0].m128_f32[3] = 0.0f;
	this->rotationMatrix.r[1].m128_f32[3] = 0.0f;
	this->rotationMatrix.r[2].m128_f32[3] = 0.0f;

	this->UpdateModelMatrix();
}

XMVECTOR GameObject::GetFrontVector(bool noY)
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

XMVECTOR GameObject::GetRightVector(bool noY)
{
	if (noY)
		return this->rightNoY;
	return this->right;
}

const XMVECTOR& GameObject::GetUpVector()
{
	return this->up;
}

void GameObject::SetFrontVector(const XMVECTOR& front)
{
	this->front = front;
}

void GameObject::SetUpVector(const XMVECTOR& up)
{
	this->up = up;
}

void GameObject::SetRightVector(const XMVECTOR& right)
{
	this->right = right;
}

void GameObject::Update(float deltaTime)
{
	if (followingTrack) {
		this->objectTrack->Follow(this, deltaTime);
	}
	if (isFollowingObject && objectToFollow != nullptr)
	{
		SetPosition(XMVectorLerp(this->positionVector, objectToFollow->GetPositionVector() + XMVectorSet(0.0f, 2.0f, 0.0f, 0.0f), 0.0005f * deltaTime));
		SetLookAtPos(objectToFollow->GetPositionVector());
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

void GameObject::SetLabel(std::string newLabel)
{
	this->label = newLabel;
}

XMMATRIX GameObject::GetModelMatrix()
{
	return this->modelMatrix;
}

XMMATRIX GameObject::GetRotationMatrix()
{
	return this->rotationMatrix;
}

void GameObject::SetRotationMatrix(XMMATRIX rotationMatrix)
{
	this->rotationMatrix = rotationMatrix;
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

bool GameObject::GetFloating()
{
	return floating;
}

void GameObject::SetFloating(bool floating)
{
	this->floating = floating;
}

void GameObject::SetObjectTrack(ObjectTrack* objectTrack) {
	this->objectTrack = objectTrack;
}

bool GameObject::GetHasParentObject()
{
	return this->hasParentObject;
}

void GameObject::SetHasParentObject(bool hasParentObject)
{
	this->hasParentObject = hasParentObject;
}

GameObject* GameObject::GetParentObject()
{
	return this->parentObject;
}

void GameObject::SetParentObject(GameObject* parentObject)
{
	this->parentObject = parentObject;
}

void GameObject::SetObjectToFollow(GameObject* objectToFollow)
{
	this->objectToFollow = objectToFollow;
}

void GameObject::SetIsFollowingObject(bool isFollowingObject)
{
	this->isFollowingObject = isFollowingObject;
}

bool GameObject::GetIsFollowingObject()
{
	return this->isFollowingObject;
}

ObjectTrack* GameObject::GetObjectTrack() {
	return this->objectTrack;
}
