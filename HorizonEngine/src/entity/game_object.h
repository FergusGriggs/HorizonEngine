
//The base class for any object in the scene. Stores information on the position, rotation and type of object

#pragma once

#include "model.h"

#include "utils/game_object_track.h"

#include "../physics/transform.h"

namespace hrzn::entity
{
	class GameObjectController;

	class GameObject
	{
	public:
		enum class Type
		{
			eBase,
			eRenderable,
			eCamera,
			eLight,
			ePointLight,
			eSpotLight,
			ePhysics,
			eNumTypes,
		};

	public:
		// Constructors
		GameObject();
		virtual ~GameObject();

		// Transform functions
		const physics::Transform& getTransform() const;
		physics::Transform&       getWritableTransform();
		void                      setTransform(const physics::Transform& transform);

		// Object track functions
		void                   setObjectTrack(GameObjectTrack* objectTrack);
		const GameObjectTrack* getObjectTrack() const;
		bool                   hasObjectTrack() const;

		void setFollowingObjectTrack(bool followingTrack);
		bool isFollowingObjectTrack() const;
	
		void  setObjectTrackDelta(float trackDelta);
		float getObjectTrackDelta() const;

		void  setObjectTrackSpeed(float trackSpeed);
		float getObjectTrackSpeed() const;

		// Float functions
		void            setFloating(bool floating);
		bool            getFloating() const;
		const XMFLOAT3& getFloatOffset() const;
		XMFLOAT3&       getWritableFloatOffset();

		// Parent functions
		void              setParentObject(const GameObject* parentObject);
		const GameObject* getParentObject() const;
		bool              hasParentObject() const;
		
		// Object follow functions
		void setObjectToFollow(const GameObject* objectToFollow);
		bool isFollowingObject() const;

		// Controller functions
		void                        setController(GameObjectController* controller);
		const GameObjectController* getController() const;
		GameObjectController*       getWritableController();

		virtual void update(float deltaTime);

		// Identification
		Type               getType() const;
		const std::string& getLabel() const;
		std::string*       getLabelPtr();
		void               setLabel(const std::string& newLabel);

		const std::vector<XMFLOAT3>& getRelativePositions() const;
		std::vector<XMFLOAT3>&       getWritableRelativePositions();

	protected:
		std::string           m_label;
		Type                  m_type;

		physics::Transform    m_transform;

		// Track information
		bool                  m_followingTrack;
		GameObjectTrack*      m_objectTrack;

		// Floating
		bool                  m_floating;
		XMFLOAT3              m_floatOffset;

		// Relative camera
		const GameObject*     m_parentObject;
		std::vector<XMFLOAT3> m_relativePositions;

		// Following objects
		const GameObject*     m_objectToFollow;

		// Controller
		GameObjectController* m_controller;
	};

	static const std::unordered_map<std::string, GameObject::Type> sc_gameObjectStringToType = {
		{ "Base",       GameObject::Type::eBase },
		{ "Renderable", GameObject::Type::eRenderable },
		{ "Camera",     GameObject::Type::eCamera },
		{ "Light",      GameObject::Type::eLight },
		{ "PointLight", GameObject::Type::ePointLight },
		{ "SpotLight",  GameObject::Type::eSpotLight },
		{ "Physics",    GameObject::Type::ePhysics },
	};

	static const std::unordered_map<GameObject::Type, std::string> sc_gameObjectTypeToString = {
		{ GameObject::Type::eBase,       "Base" },
		{ GameObject::Type::eRenderable, "Renderable" },
		{ GameObject::Type::eCamera,     "Camera" },
		{ GameObject::Type::eLight,      "Light" },
		{ GameObject::Type::ePointLight, "PointLight" },
		{ GameObject::Type::eSpotLight,  "SpotLight" },
		{ GameObject::Type::ePhysics,    "Physics" },
	};
}
