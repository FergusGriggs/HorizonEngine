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

	this->SetPosition(0.0f, 0.0f, 0.0f);
	this->SetRotation(0.0f, 0.0f, 0.0f);
	this->UpdateModelMatrix();

	return true;
}

void Light::UpdateShaderVariables(ConstantBuffer<CB_PS_pixelShader>& cb_ps_pixelShader)
{
	cb_ps_pixelShader.data.directionalLightColour = this->colour;
	cb_ps_pixelShader.data.directionalLightAmbientStrength = this->ambientStrength;
	cb_ps_pixelShader.data.directionalLightSpecularStrength = this->specularStrength;
	cb_ps_pixelShader.data.directionalLightShininess = this->shininess;

	XMStoreFloat3(&cb_ps_pixelShader.data.directionalLightDirection, this->GetFrontVector());
}

void Light::SetColour(DirectX::XMFLOAT3 colour) {
	this->colour = colour;
}

void Light::SetAmbientStrength(float ambientStrength) {
	this->ambientStrength = ambientStrength;
}

void Light::SetSpecularStrength(float specularStrength) {
	this->specularStrength = specularStrength;
}

void Light::SetShininess(float shininess) {
	this->shininess = shininess;
}

float Light::GetAmbientStrength() {
	return this->ambientStrength;
}

float Light::GetSpecularStrength() {
	return this->specularStrength;
}

float Light::GetShininess() {
	return this->shininess;
}
