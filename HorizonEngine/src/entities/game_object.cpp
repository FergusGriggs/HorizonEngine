
//Function implementations for the GameObject class

#include "game_object.h"

#include "utility/game_object_controller.h"

namespace hrzn::entity
{
	GameObject::GameObject() :
		m_label("null"),
		m_type(GameObjectType::eBase),

		m_transform(),

		m_followingTrack(false),
		m_objectTrack(nullptr),

		m_floating(false),

		m_hasParentObject(false),
		m_parentObject(nullptr),
		m_relativePositions(),

		m_isFollowingObject(false),
		m_objectToFollow(nullptr),

		m_controller(nullptr)
	{
	}

	GameObject::~GameObject()
	{
	}

	void GameObject::setTransform(const Transform& transform)
	{
		m_transform = transform;
	}

	Transform& GameObject::getTransform()
	{
		return m_transform;
	}

	void GameObject::update(float deltaTime)
	{
		if (m_followingTrack)
		{
			m_objectTrack->follow(this, deltaTime);
		}
		if (m_isFollowingObject && m_objectToFollow != nullptr)
		{
			m_transform.setPosition(XMVectorLerp(m_transform.getPositionVector(), m_objectToFollow->m_transform.getPositionVector() + XMVectorSet(0.0f, 2.0f, 0.0f, 0.0f), 0.5f * deltaTime));
			m_transform.lookAtPosition(m_objectToFollow->getTransform().getPositionVector());
		}
	}

	GameObjectType GameObject::getType()
	{
		return m_type;
	}

	const std::string& GameObject::getLabel()
	{
		return m_label;
	}

	std::string* GameObject::getLabelPtr()
	{
		return &m_label;
	}

	void GameObject::setLabel(const std::string& newLabel)
	{
		m_label = newLabel;
	}

	std::vector<XMFLOAT3>* GameObject::getRelativePositions()
	{
		return &m_relativePositions;
	}

	float GameObject::getObjectTrackDelta()
	{
		return m_objectTrack->getDelta();

	}
	void GameObject::setObjectTrackDelta(float objectTrackDelta)
	{
		m_objectTrack->setDelta(objectTrackDelta);
	}

	bool GameObject::getFollowingObjectTrack()
	{
		return m_followingTrack;
	}

	void GameObject::setFollowingObjectTrack(bool followingTrack)
	{
		m_followingTrack = followingTrack;
	}

	bool GameObject::getFloating()
	{
		return m_floating;
	}

	void GameObject::setFloating(bool floating)
	{
		m_floating = floating;
	}

	void GameObject::setObjectTrack(GameObjectTrack* objectTrack)
	{
		m_objectTrack = objectTrack;
	}

	bool GameObject::getHasParentObject()
	{
		return m_hasParentObject;
	}

	void GameObject::setHasParentObject(bool hasParentObject)
	{
		m_hasParentObject = hasParentObject;
	}

	GameObject* GameObject::getParentObject()
	{
		return m_parentObject;
	}

	void GameObject::setParentObject(GameObject* parentObject)
	{
		m_parentObject = parentObject;
	}

	void GameObject::setObjectToFollow(GameObject* objectToFollow)
	{
		objectToFollow = objectToFollow;
	}

	void GameObject::setIsFollowingObject(bool isFollowingObject)
	{
		m_isFollowingObject = isFollowingObject;
	}

	bool GameObject::getIsFollowingObject()
	{
		return m_isFollowingObject;
	}

	GameObjectController* GameObject::getController()
	{
		return m_controller;
	}

	void GameObject::setController(GameObjectController* controller)
	{
		m_controller = controller;
	}

	GameObjectTrack* GameObject::getObjectTrack()
	{
		return m_objectTrack;
	}
}
