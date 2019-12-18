//GameObject.h
//The base class for any object in the scene. Stores information on the position, rotation and type of object

#pragma once

#include <DirectXCollision.h>

#include "Model.h"
#include "utility/ObjectTrack.h"

enum class GameObjectType {
	BASE,
	RENDERABLE,
	CAMERA,
	LIGHT,
	POINT_LIGHT,
	SPOT_LIGHT
};

class GameObject
{
public:
	GameObject();

	const XMVECTOR& GetPositionVector() const;
	const XMFLOAT3& GetPositionFloat3() const;

	void SetPosition(const XMVECTOR& position);
	void SetPosition(const XMFLOAT3& position);
	void SetPosition(float x, float y, float z);
	void AdjustPosition(const XMVECTOR& diff);
	void AdjustPosition(const XMFLOAT3& diff);
	void AdjustPosition(float x, float y, float z);

	void SetRotation(const XMVECTOR& rotation);
	void SetRotation(const XMFLOAT3& rotation);
	void SetRotation(float x, float y, float z);

	void SetLookAtPos(XMVECTOR lookAtPos);
	void SetLookAtPos(XMFLOAT3 lookAtPos);

	void RotateAxisVectors(XMVECTOR axis, float angle);
	void CopyAxisVectorsFrom(GameObject* gameObject);

	XMVECTOR GetFrontVector(bool noY = false);
	const XMVECTOR& GetBackVector(bool noY = false);
	const XMVECTOR& GetLeftVector(bool noY = false);
	XMVECTOR GetRightVector(bool noY = false);
	const XMVECTOR& GetUpVector();

	void SetFrontVector(const XMVECTOR& front);
	void SetUpVector(const XMVECTOR& up);
	void SetRightVector(const XMVECTOR& right);

	void SetObjectTrack(ObjectTrack* objectTrack);
	ObjectTrack* GetObjectTrack();

	bool GetFollowingObjectTrack();
	void SetFollowingObjectTrack(bool followingTrack);

	bool GetFloating();
	void SetFloating(bool floating);

	float GetObjectTrackDelta();
	void SetObjectTrackDelta(float trackDelta);

	bool GetHasParentObject();
	void SetHasParentObject(bool hasParentObject);
	GameObject* GetParentObject();
	void SetParentObject(GameObject* parentObject);

	void SetObjectToFollow(GameObject* objectToFollow);
	void SetIsFollowingObject(bool isFollowingObject);
	bool GetIsFollowingObject();

	virtual void Update(float deltaTime);

	GameObjectType GetType();
	std::string GetLabel();
	void SetLabel(std::string newLabel);

	XMMATRIX GetModelMatrix();
	XMMATRIX GetRotationMatrix();
	void SetRotationMatrix(XMMATRIX rotationMatrix);

	virtual void UpdateModelMatrix();
	std::vector<XMVECTOR>* GetRelativePositions();

protected:
	virtual void SetObjectDelta(float objectTrackDelta);

	void CreateAxisVectorsFromRotMat();

	XMVECTOR positionVector;
	XMFLOAT3 position;

	const XMVECTOR DEFAULT_FRONT_VECTOR = XMVectorSet(0.0f, 0.0f, 1.0f, 0.0f);
	const XMVECTOR DEFAULT_UP_VECTOR = XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);
	const XMVECTOR DEFAULT_RIGHT_VECTOR = XMVectorSet(1.0f, 0.0f, 0.0f, 0.0f);

	XMVECTOR front = DEFAULT_FRONT_VECTOR;
	XMVECTOR right = DEFAULT_RIGHT_VECTOR;
	XMVECTOR up = DEFAULT_UP_VECTOR;

	XMVECTOR frontNoY;
	XMVECTOR rightNoY;

	bool followingTrack = false;
	ObjectTrack* objectTrack = nullptr;

	bool floating = false;

	bool hasParentObject = false;
	GameObject* parentObject = nullptr;

	bool isFollowingObject = false;
	GameObject* objectToFollow = nullptr;

	GameObjectType type;

	std::string label;

	XMMATRIX modelMatrix = XMMatrixIdentity();
	XMMATRIX rotationMatrix = XMMatrixIdentity();

	std::vector<XMVECTOR> relativePositions;
};

