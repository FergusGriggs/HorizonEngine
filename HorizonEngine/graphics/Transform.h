#pragma once

#include <DirectXMath.h>

using namespace DirectX;

#define DEFAULT_FRONT_VECTOR XMVectorSet(0.0f, 0.0f, 1.0f, 0.0f)
#define DEFAULT_UP_VECTOR XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f)
#define DEFAULT_RIGHT_VECTOR XMVectorSet(1.0f, 0.0f, 0.0f, 0.0f)
#define DEFAULT_POSITION XMVectorSet(0.0f, 0.0f, 0.0f, 1.0f)

class Transform
{
private:
	// Position info
	XMVECTOR positionVector = DEFAULT_POSITION;
	XMFLOAT3 positionFloat3 = XMFLOAT3(0.0f, 0.0f, 0.0f);

	// Orientation info
	XMVECTOR orientation = XMQuaternionIdentity();
	XMVECTOR up = DEFAULT_UP_VECTOR;
	XMVECTOR front = DEFAULT_FRONT_VECTOR;
	XMVECTOR right = DEFAULT_RIGHT_VECTOR;
	XMMATRIX rotationMatrix = XMMatrixIdentity();

public:
	//Constructors
	Transform();
	Transform(const XMVECTOR& position, const XMVECTOR& orientationQuaternion);
	Transform(const Transform& transform);

	// Copy functions
	void CopyOrientationFrom(const Transform& otherTransform);
	void CopyPositionFrom(const Transform& otherTransform);

	// Position set functions
	void SetPosition(const XMVECTOR& position);
	void SetPosition(const XMFLOAT3& position);
	void SetPosition(float x, float y, float z);

	//Adjust position functions
	void AdjustPosition(const XMVECTOR& difference);
	void AdjustPosition(const XMFLOAT3& difference);
	void AdjustPosition(float diffX, float diffY, float diffZ);

	// Position get functions
	const XMVECTOR& GetPositionVector() const;
	const XMFLOAT3& GetPositionFloat3() const;

	// Orientation set functions
	void SetOrientationQuaternion(const XMVECTOR& quaternion, bool updateAll = true);
	void SetOrientationRotationMatrix(const XMMATRIX& rotationMatrix, bool updateAll = true);
	void SetOrientationRollPitchYaw(float roll, float pitch, float yaw);

	//Other orientation functions
	void LookAtPos(XMVECTOR lookAtPos);
	void LookAtPos(XMFLOAT3 lookAtPos);
	void RotateUsingAxis(XMVECTOR axis, float angle);

	// Orientation get functions
	const XMVECTOR& GetOrientation() const;
	const XMFLOAT4& GetOrientationFloat4() const;
	const XMMATRIX& GetRotationMatrix() const;

	const XMVECTOR& GetFrontVector() const;
	const XMVECTOR& GetBackVector() const;
	const XMVECTOR& GetUpVector() const;
	const XMVECTOR& GetDownVector() const;
	const XMVECTOR& GetRightVector() const;
	const XMVECTOR& GetLeftVector() const;

	void UpdateAxisVectorsUsingRotationMatrix();
};

