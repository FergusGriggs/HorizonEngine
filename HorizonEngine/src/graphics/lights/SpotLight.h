//SpotLight.h
//Child class of Light which also stores attenuation information and cutoff values so a feathered cone of light can be modelled

#pragma once

#include "Light.h"

class SpotLight : public Light
{
public:
	SpotLight();

	void UpdateShaderVariables(ConstantBuffer<CB_PS_pixelShader>& cb_ps_pixelShader, int lightIndex = 0) override;
	float* GetAttenuationConstantPtr();
	float* GetAttenuationLinearPtr();
	float* GetAttenuationQuadraticPtr();

	float* GetInnerCutoffPtr();
	float* GetOuterCutoffPtr();

	float GetInnerCutoff();
	float GetOuterCutoff();

	void SetInnerCutoff(float innerCutoff);
	void SetOuterCutoff(float outerCutoff);

	/*float attenuationConstant = 1.0f;
	float attenuationLinear = 0.045f;
	float attenuationQuadratic = 0.0075f;

	float innerCutoff = 1.0f;
	float outerCutoff = 0.95f;*/

	float attenuationConstant = 0.762f;
	float attenuationLinear = 0.044f;
	float attenuationQuadratic = 0.0015f;

	float innerCutoff = 45.0f;
	float outerCutoff = 35.0f;
};

