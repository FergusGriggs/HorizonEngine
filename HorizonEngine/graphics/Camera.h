//Camera.h
//Primarily modifies view and projection matrices so that a scene can be rendered from its perspective

#pragma once

#include <DirectXMath.h>
#include "GameObject.h"
#include "utility/ObjectTrack.h"

class CameraTrack;

using namespace DirectX;

class Camera : public GameObject
{
public:
	Camera();
	void Update(float deltaTime) override;
	void SetProjectionValues(float fovDegrees, float aspectRatio, float nearZ, float farZ);
	void Zoom(float fovDiff);
	float GetZoom();

	const XMMATRIX& GetViewMatrix() const;
	const XMMATRIX& GetProjectionMatrix() const;

	void ComputeMouseToWorldVectorDirection(float mouseNDCX, float mouseNDCY);
	XMVECTOR GetMouseToWorldVectorDirection();

	XMFLOAT2 GetNDCFrom3DPos(XMVECTOR objectPosition);

private:
	void UpdateModelMatrix() override;

	XMMATRIX view;
	XMMATRIX projection;

	float fov;
	float aspectRatio;
	float nearZ;
	float farZ;

	XMVECTOR mouseToWorldVectorDirection;
};

