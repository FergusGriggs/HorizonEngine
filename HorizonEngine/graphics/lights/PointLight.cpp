//PointLight.cpp
//Function implementation for the PointLight class

#include "PointLight.h"

PointLight::PointLight() {
	this->type = GameObjectType::POINT_LIGHT;
}

void PointLight::UpdateShaderVariables(ConstantBuffer<CB_PS_pixelShader>& cb_ps_pixelShader)
{
	cb_ps_pixelShader.data.pointLightColour = this->colour;
	cb_ps_pixelShader.data.pointLightAmbientStrength = this->ambientStrength;

	cb_ps_pixelShader.data.pointLightAttenuationConstant = this->attenuationConstant;
	cb_ps_pixelShader.data.pointLightAttenuationLinear = this->attenuationLinear;
	cb_ps_pixelShader.data.pointLightAttenuationQuadratic = this->attenuationQuadratic;
	cb_ps_pixelShader.data.pointLightSpecularStrength = this->specularStrength;

	cb_ps_pixelShader.data.pointLightPosition = this->GetPositionFloat3();
	cb_ps_pixelShader.data.pointLightShininess = this->shininess;
}
