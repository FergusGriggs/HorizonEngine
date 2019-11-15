//RenderableGameObject.h
//Child class of GameObject which stores an additional data to allow the object to be rendered and transformed

#pragma once

#include "GameObject.h"

class RenderableGameObject : public GameObject
{
public:
	RenderableGameObject();
	bool Initialize(std::string label, const std::string& filePath, ID3D11Device* device, ID3D11DeviceContext* deviceContext, ConstantBuffer<CB_VS_vertexShader>& cb_vs_vertexShader);
	void Draw(const XMMATRIX& viewProjectionMatrix);

	float GetRayIntersectDist(XMVECTOR rayOrigin, XMVECTOR rayDirection);

	XMFLOAT3 GetScale();
	Model* GetModel();

	void SetScale(XMFLOAT3 scale);

protected:
	Model model;
	Model* axisModel;

	XMFLOAT3 scale;

	BoundingSphere boundingSphere;
};
