
//Function implementations for the GameObject class

#include "game_object.h"

#include "utils/game_object_controller.h"

namespace hrzn::entity
{
	GameObject::GameObject() :
		m_label("null"),
		m_type(GameObjectType::eBase),

		m_transform(),

		m_followingTrack(false),
		m_objectTrack(nullptr),

		m_floating(false),
		m_floatOffset(XMFLOAT3(0.0f, 0.0f, 0.0f)),

		m_parentObject(nullptr),
		m_relativePositions(),

		m_objectToFollow(nullptr),

		m_controller(nullptr)
	{
	}

	GameObject::~GameObject()
	{
	}

	const physics::Transform& GameObject::getTransform() const
	{
		return m_transform;
	}

	physics::Transform& GameObject::getWritableTransform()
	{
		return m_transform;
	}

	void GameObject::setTransform(const physics::Transform& transform)
	{
		m_transform = transform;
	}

	void GameObject::update(float deltaTime)
	{
		if (m_followingTrack && m_objectTrack != nullptr)
		{
			m_objectTrack->follow(this, deltaTime);
		}

		if (m_objectToFollow != nullptr)
		{
			m_transform.setPosition(XMVectorLerp(m_transform.getPositionVector(), m_objectToFollow->m_transform.getPositionVector() + XMVectorSet(0.0f, 2.0f, 0.0f, 0.0f), 0.5f * deltaTime));
			m_transform.lookAtPosition(m_objectToFollow->getTransform().getPositionVector());
		}
	}

	GameObjectType GameObject::getType() const
	{
		return m_type;
	}

	void GameObject::setLabel(const std::string& newLabel)
	{
		m_label = newLabel;
	}

	const std::string& GameObject::getLabel() const
	{
		return m_label;
	}

	std::string* GameObject::getLabelPtr()
	{
		return &m_label;
	}

	const std::vector<XMFLOAT3>& GameObject::getRelativePositions() const
	{
		return m_relativePositions;
	}

	std::vector<XMFLOAT3>& GameObject::getWritableRelativePositions()
	{
		return m_relativePositions;
	}

	void GameObject::setFollowingObjectTrack(bool followingTrack)
	{
		m_followingTrack = followingTrack;
	}

	bool GameObject::isFollowingObjectTrack() const
	{
		return m_followingTrack;
	}

	void GameObject::setObjectTrackDelta(float objectTrackDelta)
	{
		m_objectTrack->setDelta(objectTrackDelta);
	}

	float GameObject::getObjectTrackDelta() const
	{
		return m_objectTrack->getDelta();
	}

	void GameObject::setFloating(bool floating)
	{
		m_floating = floating;
	}

	bool GameObject::getFloating() const
	{
		return m_floating;
	}

	const XMFLOAT3& GameObject::getFloatOffset() const
	{
		return m_floatOffset;
	}

	XMFLOAT3& GameObject::getWritableFloatOffset()
	{
		return m_floatOffset;
	}

	void GameObject::setObjectTrack(GameObjectTrack* objectTrack)
	{
		m_objectTrack = objectTrack;
	}

	bool GameObject::hasParentObject() const
	{
		return m_parentObject != nullptr;
	}

	const GameObject* GameObject::getParentObject() const
	{
		return m_parentObject;
	}

	void GameObject::setParentObject(const GameObject* parentObject)
	{
		m_parentObject = parentObject;
	}

	void GameObject::setObjectToFollow(const GameObject* objectToFollow)
	{
		m_objectToFollow = objectToFollow;
	}

	bool GameObject::isFollowingObject() const
	{
		return m_objectToFollow != nullptr;
	}

	const GameObjectController* GameObject::getController() const
	{
		return m_controller;
	}

	GameObjectController* GameObject::getWritableController()
	{
		return m_controller;
	}

	void GameObject::setController(GameObjectController* controller)
	{
		m_controller = controller;
	}

	const GameObjectTrack* GameObject::getObjectTrack() const
	{
		return m_objectTrack;
	}
	bool GameObject::hasObjectTrack() const
	{
		return m_objectTrack != nullptr;
	}
}
