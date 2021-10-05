//GameObject.h
//The base class for any object in the scene. Stores information on the position, rotation and type of object

#pragma once

#include "Model.h"
#include "utility/ObjectTrack.h"
#include "Transform.h"

enum class GameObjectType {
	BASE,
	RENDERABLE,
	CAMERA,
	LIGHT,
	POINT_LIGHT,
	SPOT_LIGHT,
	PHYSICS,
};

class Controller;

class GameObject
{
public:
	// Constructors
	GameObject();
	virtual ~GameObject();

	// Transform functions
	void SetTransform(Transform* transform);
	Transform* GetTransform();

	// Object track functions
	void SetObjectTrack(ObjectTrack* objectTrack);
	ObjectTrack* GetObjectTrack();

	bool GetFollowingObjectTrack();
	void SetFollowingObjectTrack(bool followingTrack);
	
	float GetObjectTrackDelta();
	void SetObjectTrackDelta(float trackDelta);

	// Float functions
	bool GetFloating();
	void SetFloating(bool floating);

	// Parent functions
	void SetParentObject(GameObject* parentObject);
	GameObject* GetParentObject();
	bool GetHasParentObject();
	void SetHasParentObject(bool hasParentObject);

	// Object follow functions
	void SetObjectToFollow(GameObject* objectToFollow);
	void SetIsFollowingObject(bool isFollowingObject);
	bool GetIsFollowingObject();

	// Controller functions
	Controller* GetController();
	void SetController(Controller* controller);

	virtual void Update(float deltaTime);

	//Identification
	GameObjectType GetType();
	std::string GetLabel();
	void SetLabel(std::string newLabel);

	std::vector<XMVECTOR>* GetRelativePositions();

protected:
	virtual void SetObjectDelta(float objectTrackDelta);

	GameObjectType type;

	Transform transform;

	//Track information
	bool followingTrack = false;
	ObjectTrack* objectTrack = nullptr;

	//Floating
	bool floating = false;

	//Relative Camera
	bool hasParentObject = false;
	GameObject* parentObject = nullptr;
	std::vector<XMVECTOR> relativePositions;

	//Following objects
	bool isFollowingObject = false;
	GameObject* objectToFollow = nullptr;

	//Controller
	Controller* controller = nullptr;

	std::string label;
};

#include "utility/Controller.h"