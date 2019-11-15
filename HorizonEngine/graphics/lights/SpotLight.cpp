//SpotLight.cpp
//Function implementation for the SpotLight class

#include "SpotLight.h"

SpotLight::SpotLight() {
	this->type = GameObjectType::SPOT_LIGHT;
}

void SpotLight::UpdateShaderVariables(ConstantBuffer<CB_PS_pixelShader>& cb_ps_pixelShader) {
	cb_ps_pixelShader.data.spotLightColour = this->colour;
	cb_ps_pixelShader.data.spotLightAmbientStrength = this->ambientStrength;

	cb_ps_pixelShader.data.spotLightAttenuationConstant = this->attenuationConstant;
	cb_ps_pixelShader.data.spotLightAttenuationLinear = this->attenuationLinear;
	cb_ps_pixelShader.data.spotLightAttenuationQuadratic = this->attenuationQuadratic;
	cb_ps_pixelShader.data.spotLightSpecularStrength = this->specularStrength;

	cb_ps_pixelShader.data.spotLightPosition = this->GetPositionFloat3();
	XMStoreFloat3(&cb_ps_pixelShader.data.spotLightDirection, this->GetFrontVector());
	cb_ps_pixelShader.data.spotLightShininess = this->shininess;

	cb_ps_pixelShader.data.spotLightInnerCutoff = this->innerCutoff;
	cb_ps_pixelShader.data.spotLightOuterCutoff = this->outerCutoff;
}