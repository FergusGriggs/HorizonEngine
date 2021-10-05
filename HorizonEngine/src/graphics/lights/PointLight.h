//PointLight.h
//Child class of Light which also stores attenuation information

#pragma once

#include "Light.h"

class PointLight : public Light
{
public:
	PointLight();

	void UpdateShaderVariables(ConstantBuffer<CB_PS_pixelShader>& cb_ps_pixelShader, int lightIndex = 0) override;
	float* GetAttenuationConstantPtr();
	float* GetAttenuationLinearPtr();
	float* GetAttenuationQuadraticPtr();

	/*float attenuationConstant = 1.0f;
	float attenuationLinear = 0.09f;
	float attenuationQuadratic = 0.032f;*/

	float attenuationConstant = 0.706f;
	float attenuationLinear = 0.073f;
	float attenuationQuadratic = 0.011f;
};

