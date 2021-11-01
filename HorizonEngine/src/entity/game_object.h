
//The base class for any object in the scene. Stores information on the position, rotation and type of object

#pragma once

#include "model.h"

#include "utils/game_object_track.h"

#include "../physics/transform.h"

namespace hrzn::entity
{
	enum class GameObjectType {
		eBase,
		eRenderable,
		eCamera,
		eLight,
		ePointLight,
		eSpotLight,
		ePhysics,
	};

	class GameObjectController;

	class GameObject
	{
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
		GameObjectType     getType() const;
		const std::string& getLabel() const;
		std::string*       getLabelPtr();
		void               setLabel(const std::string& newLabel);

		const std::vector<XMFLOAT3>& getRelativePositions() const;
		std::vector<XMFLOAT3>&       getWritableRelativePositions();

	protected:
		std::string        m_label;
		GameObjectType     m_type;

		physics::Transform m_transform;

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
}
