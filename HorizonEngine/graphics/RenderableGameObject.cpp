#include "RenderableGameObject.h"

RenderableGameObject::RenderableGameObject() {
	this->type = GameObjectType::RENDERABLE;
}

bool RenderableGameObject::Initialize(std::string label, const std::string& filePath, ID3D11Device* device, ID3D11DeviceContext* deviceContext, ConstantBuffer<CB_VS_vertexShader>& cb_vs_vertexShader)
{
	this->label = label;

	if (!this->model.Initialize(filePath, device, deviceContext, cb_vs_vertexShader)) {
		return false;
	}

	this->SetPosition(0.0f, 0.0f, 0.0f);

	this->scale = XMFLOAT3(1.0f, 1.0f, 1.0f);

	return true;
}

void RenderableGameObject::Draw(const XMMATRIX& viewProjectionMatrix)
{
	if (this->type == GameObjectType::RENDERABLE) {
		model.Draw(XMMatrixScaling(this->scale.x, this->scale.y, this->scale.z) * this->modelMatrix, viewProjectionMatrix);
	}
	else {
		model.Draw(this->modelMatrix, viewProjectionMatrix);
	}
}

float RenderableGameObject::GetRayIntersectDist(XMVECTOR rayOrigin, XMVECTOR rayDirection)
{
	boundingSphere.Center = position;
	boundingSphere.Radius = 0.35f;
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