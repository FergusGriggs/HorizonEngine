
//The base class for any object in the scene. Stores information on the position, rotation and type of object

#pragma once

#include "Model.h"
#include "utility/game_object_track.h"
#include "transform.h"

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
		void             setTransform(const Transform& transform);
		Transform& getTransform();

		// Object track functions
		void             setObjectTrack(GameObjectTrack* objectTrack);
		GameObjectTrack* getObjectTrack();

		bool getFollowingObjectTrack();
		void setFollowingObjectTrack(bool followingTrack);

		float getObjectTrackDelta();
		void  setObjectTrackDelta(float trackDelta);

		// Float functions
		bool      getFloating();
		void      setFloating(bool floating);
		XMFLOAT3& getFloatOffset();

		// Parent functions
		void        setParentObject(GameObject* parentObject);
		GameObject* getParentObject();
		bool        getHasParentObject();
		void        setHasParentObject(bool hasParentObject);

		// Object follow functions
		void setObjectToFollow(GameObject* objectToFollow);
		void setIsFollowingObject(bool isFollowingObject);
		bool getIsFollowingObject();

		// Controller functions
		GameObjectController* getController();
		void                  setController(GameObjectController* controller);

		virtual void update(float deltaTime);

		//Identification
		GameObjectType     getType();
		const std::string& getLabel();
		std::string*       getLabelPtr();
		void               setLabel(const std::string& newLabel);

		std::vector<XMFLOAT3>* getRelativePositions();

	protected:
		std::string      m_label;
		GameObjectType   m_type;

		Transform        m_transform;

		//Track information
		bool             m_followingTrack;
		GameObjectTrack* m_objectTrack;

		//Floating
		bool             m_floating;
		XMFLOAT3         m_floatOffset;

		//Relative Camera
		bool                  m_hasParentObject;
		GameObject*           m_parentObject;
		std::vector<XMFLOAT3> m_relativePositions;

		//Following objects
		bool                  m_isFollowingObject;
		GameObject*           m_objectToFollow;

		//Controller
		GameObjectController* m_controller;
	};
}
