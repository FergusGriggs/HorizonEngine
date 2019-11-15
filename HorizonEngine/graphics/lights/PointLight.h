//PointLight.h
//Child class of Light which also stores attenuation information

#pragma once

#include "Light.h"

class PointLight : public Light
{
public:
	PointLight();

	void UpdateShaderVariables(ConstantBuffer<CB_PS_pixelShader>& cb_ps_pixelShader) override;

	float attenuationConstant = 1.0f;
	float attenuationLinear = 0.09f;
	float attenuationQuadratic = 0.032f;
};

