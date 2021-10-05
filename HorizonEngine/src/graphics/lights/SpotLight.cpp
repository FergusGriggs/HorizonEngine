//SpotLight.cpp
//Function implementation for the SpotLight class

#include "SpotLight.h"

SpotLight::SpotLight() {
	this->type = GameObjectType::SPOT_LIGHT;
}

void SpotLight::UpdateShaderVariables(ConstantBuffer<CB_PS_pixelShader>& cb_ps_pixelShader, int lightIndex) {
	//NORMALIZE
	XMVECTOR colour = XMVector3Normalize(XMLoadFloat3(&this->colour)) * 5.0f;
	XMStoreFloat3(&cb_ps_pixelShader.data.spotLights[lightIndex].colour, colour);

	cb_ps_pixelShader.data.spotLights[lightIndex].attenuationConstant = this->attenuationConstant;
	cb_ps_pixelShader.data.spotLights[lightIndex].attenuationLinear = this->attenuationLinear;
	cb_ps_pixelShader.data.spotLights[lightIndex].attenuationQuadratic = this->attenuationQuadratic;

	cb_ps_pixelShader.data.spotLights[lightIndex].position = this->transform.GetPositionFloat3();
	XMStoreFloat3(&cb_ps_pixelShader.data.spotLights[lightIndex].direction, this->transform.GetFrontVector());

	cb_ps_pixelShader.data.spotLights[lightIndex].innerCutoff = this->innerCutoff;
	cb_ps_pixelShader.data.spotLights[lightIndex].outerCutoff = this->outerCutoff;
}

float* SpotLight::GetAttenuationConstantPtr()
{
	return &attenuationConstant;
}

float* SpotLight::GetAttenuationLinearPtr()
{
	return &attenuationLinear;
}

float* SpotLight::GetAttenuationQuadraticPtr()
{
	return &attenuationQuadratic;
}

float* SpotLight::GetInnerCutoffPtr()
{
	return &innerCutoff;
}

float* SpotLight::GetOuterCutoffPtr()
{
	return &outerCutoff;
}

float SpotLight::GetInnerCutoff()
{
	return this->innerCutoff;
}

float SpotLight::GetOuterCutoff()
{
	return this->outerCutoff;
}

void SpotLight::SetInnerCutoff(float innerCutoff)
{
	this->innerCutoff = innerCutoff;
}

void SpotLight::SetOuterCutoff(float outerCutoff)
{
	this->outerCutoff = outerCutoff;
}