//Light.cpp
//Function implementations for the Light class

#include "Light.h"

Light::Light()
{
	this->type = GameObjectType::LIGHT;
}

bool Light::Initialize(std::string label, ID3D11Device* device, ID3D11DeviceContext* deviceContext, ResourceManager* resourceManager)
{
	this->label = label;

	this->model = resourceManager->GetModelPtr("res/models/light.fbx");
	if (this->model == nullptr)
		return false;

	this->transform.SetPosition(XMVectorSet(0.0f, 0.0f, 0.0f, 1.0f));
	this->transform.SetOrientationQuaternion(XMQuaternionIdentity());

	return true;
}

void Light::UpdateShaderVariables(ConstantBuffer<CB_PS_pixelShader>& cb_ps_pixelShader, int lightIndex)
{
	cb_ps_pixelShader.data.directionalLight.colour = this->colour;
	cb_ps_pixelShader.data.directionalLight.ambientStrength = this->ambientStrength;

	XMStoreFloat3(&cb_ps_pixelShader.data.directionalLight.direction, this->transform.GetFrontVector());
}

void Light::SetColour(DirectX::XMFLOAT3 colour) {
	this->colour = colour;
}

DirectX::XMFLOAT3 Light::GetColour()
{
	return this->colour;
}

void Light::SetAmbientStrength(float ambientStrength) {
	this->ambientStrength = ambientStrength;
}

float Light::GetAmbientStrength() {
	return this->ambientStrength;
}