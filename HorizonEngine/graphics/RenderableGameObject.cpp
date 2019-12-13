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

	this->SetPosition(0.0f, 0.0f, 0.0f);

	this->scale = XMFLOAT3(1.0f, 1.0f, 1.0f);

	return true;
}

void RenderableGameObject::Draw(const XMMATRIX& viewProjectionMatrix, ConstantBuffer<CB_VS_vertexShader>* cb_vs_vertexShader)
{
	if (this->type == GameObjectType::RENDERABLE) {
		model->Draw(XMMatrixScaling(this->scale.x, this->scale.y, this->scale.z) * this->modelMatrix, viewProjectionMatrix, cb_vs_vertexShader);
	}
	else {
		model->Draw(this->modelMatrix, viewProjectionMatrix, cb_vs_vertexShader);
	}
}

float RenderableGameObject::GetRayIntersectDist(XMVECTOR rayOrigin, XMVECTOR rayDirection)
{
	boundingSphere.Center = position;
	if (label == "Boat") {
		boundingSphere.Radius = 0.0f;
	}
	boundingSphere.Radius = this->model->GetHitRadius();
	float distance;
	boundingSphere.Intersects(rayOrigin, rayDirection, distance);
	if (distance > 0.0f) {
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

