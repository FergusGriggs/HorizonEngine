#pragma once

#include <DirectXCollision.h>

#include "Model.h"
#include "ObjectTrack.h"

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

	void SetLookAtPos(XMFLOAT3 lookAtPos);

	void RotateAxisVectors(XMVECTOR axis, float angle);
	void CopyAxisVectorsFrom(GameObject* gameObject);

	const XMVECTOR& GetFrontVector(bool noY = false);
	const XMVECTOR& GetBackVector(bool noY = false);
	const XMVECTOR& GetLeftVector(bool noY = false);
	const XMVECTOR& GetRightVector(bool noY = false);
	const XMVECTOR& GetUpVector();

	void SetObjectTrack(ObjectTrack* objectTrack);
	ObjectTrack* GetObjectTrack();

	bool GetFollowingObjectTrack();
	void SetFollowingObjectTrack(bool followingTrack);

	float GetObjectTrackDelta();
	void SetObjectTrackDelta(float trackDelta);

	virtual void Update(float deltaTime);

	GameObjectType GetType();
	std::string GetLabel();

	XMMATRIX GetModelMatrix();
	XMMATRIX GetRotationMatrix();
	
	virtual void UpdateModelMatrix();

protected:
	virtual void SetObjectDelta(float objectTrackDelta);

	void CreateAxisVectorsFromRotMat();

	XMVECTOR positionVector;
	XMFLOAT3 position;

	const XMVECTOR DEFAULT_FRONT_VECTOR = XMVectorSet(0.0f, 0.0f, 1.0f, 0.0f);
	const XMVECTOR DEFAULT_UP_VECTOR = XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);
	const XMVECTOR DEFAULT_RIGHT_VECTOR = XMVectorSet(1.0f, 0.0f, 0.0f, 0.0f);

	XMVECTOR front;
	XMVECTOR right;
	XMVECTOR up;

	XMVECTOR frontNoY;
	XMVECTOR rightNoY;

	ObjectTrack* objectTrack;

	bool followingTrack;

	GameObjectType type;

	std::string label;

	XMMATRIX modelMatrix = XMMatrixIdentity();
	XMMATRIX rotationMatrix = XMMatrixIdentity();
};

