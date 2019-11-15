//SpotLight.h
//Child class of Light which also stores attenuation information and cutoff values so a feathered cone of light can be modelled

#pragma once

#include "Light.h"

class SpotLight : public Light
{
public:
	SpotLight();

	void UpdateShaderVariables(ConstantBuffer<CB_PS_pixelShader>& cb_ps_pixelShader) override;

	float attenuationConstant = 1.0f;
	float attenuationLinear = 0.045f;
	float attenuationQuadratic = 0.0075f;

	float innerCutoff = (60.0f / 180.0f) * XM_PI;
	float outerCutoff = (50.0f / 180.0f) * XM_PI;
};

