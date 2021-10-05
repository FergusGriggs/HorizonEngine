//RenderableGameObject.cpp
//Function implementations for the RenderableGameObject class

#include "RenderableGameObject.h"

RenderableGameObject::RenderableGameObject() {
	this->type = GameObjectType::RENDERABLE;
}

bool RenderableGameObject::Initialize(std::string label, const std::string& filePath, ID3D11Device* device, ID3D11DeviceContext* deviceContext, ResourceManager* resourceManager)
{
	this->label = label;

	this->model = resourceManager->GetModelPtr(filePath);

	if (this->model == nullptr) {
		return false;
	}

	this->transform.SetPosition(DEFAULT_POSITION);

	this->scale = XMFLOAT3(1.0f, 1.0f, 1.0f);

	return true;
}

void RenderableGameObject::Draw(const XMMATRIX& viewProjectionMatrix, ConstantBuffer<CB_VS_vertexShader>* cb_vs_vertexShader)
{
	XMFLOAT3 objectPosition = this->transform.GetPositionFloat3();

	if (this->type == GameObjectType::RENDERABLE) {
		model->Draw(XMMatrixScaling(this->scale.x, this->scale.y, this->scale.z) * this->transform.GetRotationMatrix() * XMMatrixTranslation(objectPosition.x, objectPosition.y, objectPosition.z) , viewProjectionMatrix, cb_vs_vertexShader);
	}
	else {
		model->Draw(this->transform.GetRotationMatrix() * XMMatrixTranslation(objectPosition.x, objectPosition.y, objectPosition.z) , viewProjectionMatrix, cb_vs_vertexShader);
	}
}

float RenderableGameObject::GetRayIntersectDist(XMVECTOR rayOrigin, XMVECTOR rayDirection)
{
	BoundingBox objectBoundingBox = this->model->GetBoundingBox();
	XMFLOAT3 objectPosition = this->transform.GetPositionFloat3();
	objectBoundingBox.Center.x += objectPosition.x;
	objectBoundingBox.Center.y += objectPosition.y;
	objectBoundingBox.Center.z += objectPosition.z;

	XMFLOAT4 orientationFloat4;
	XMStoreFloat4(&orientationFloat4, this->transform.GetOrientation());

	BoundingOrientedBox orientedObjectBoundingBox = BoundingOrientedBox(objectBoundingBox.Center, objectBoundingBox.Extents, orientationFloat4);
	
	float distance;
	orientedObjectBoundingBox.Intersects(rayOrigin, rayDirection, distance);
	if (distance > 0.0f)
	{
		// Do face check
		return distance;
	}
	return FLT_MAX;
}

XMFLOAT3 RenderableGameObject::GetScale()
{
	return this->scale;
}

void RenderableGameObject::SetScale(XMFLOAT3 scale)
{
	this->scale = scale;
}

Model* RenderableGameObject::GetModel() {
	return this->model;
}

