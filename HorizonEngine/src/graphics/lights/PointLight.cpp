//PointLight.cpp
//Function implementation for the PointLight class

#include "PointLight.h"

PointLight::PointLight() {
	this->type = GameObjectType::POINT_LIGHT;
}

void PointLight::UpdateShaderVariables(ConstantBuffer<CB_PS_pixelShader>& cb_ps_pixelShader, int lightIndex)
{
	cb_ps_pixelShader.data.pointLights[lightIndex].colour = this->colour;

	cb_ps_pixelShader.data.pointLights[lightIndex].attenuationConstant = this->attenuationConstant;
	cb_ps_pixelShader.data.pointLights[lightIndex].attenuationLinear = this->attenuationLinear;
	cb_ps_pixelShader.data.pointLights[lightIndex].attenuationQuadratic = this->attenuationQuadratic;

	cb_ps_pixelShader.data.pointLights[lightIndex].position = this->transform.GetPositionFloat3();
}

float* PointLight::GetAttenuationConstantPtr()
{
	return &attenuationConstant;
}

float* PointLight::GetAttenuationLinearPtr()
{
	return &attenuationLinear;
}

float* PointLight::GetAttenuationQuadraticPtr()
{
	return &attenuationQuadratic;
}