//GameObject.cpp
//Function implementations for the GameObject class

#include "GameObject.h"

GameObject::GameObject()
{
	this->type = GameObjectType::BASE;
}

GameObject::~GameObject()
{
	
}

void GameObject::SetTransform(Transform* transform)
{
	this->transform = *transform;
}

Transform* GameObject::GetTransform()
{
	return &this->transform;
}

void GameObject::Update(float deltaTime)
{
	if (followingTrack) {
		this->objectTrack->Follow(this, deltaTime);
	}
	if (isFollowingObject && objectToFollow != nullptr)
	{
		this->transform.SetPosition(XMVectorLerp(this->transform.GetPositionVector(), objectToFollow->transform.GetPositionVector() + XMVectorSet(0.0f, 2.0f, 0.0f, 0.0f), 0.5f * deltaTime));
		this->transform.LookAtPos(objectToFollow->GetTransform()->GetPositionVector());
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

std::vector<XMVECTOR>* GameObject::GetRelativePositions()
{
	return &this->relativePositions;
}

void GameObject::SetObjectDelta(float delta)
{
	assert("SetDelta() must be overridden." && 0);
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

Controller* GameObject::GetController()
{
	return this->controller;
}

void GameObject::SetController(Controller* controller)
{
	this->controller = controller;
}

ObjectTrack* GameObject::GetObjectTrack() {
	return this->objectTrack;
}
