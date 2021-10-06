
//Child class of Light which also stores attenuation information

#pragma once

#include "light_game_object.h"

namespace hrzn::entity
{
	class PointLightGameObject : public LightGameObject
	{
	public:
		PointLightGameObject();

		void   updateShaderVariables(gfx::ConstantBuffer<gfx::CB_PS_pixelShader>& cb_ps_pixelShader, int lightIndex = 0) override;
		float* getAttenuationConstantPtr();
		float* getAttenuationLinearPtr();
		float* getAttenuationQuadraticPtr();

		/*float attenuationConstant = 1.0f;
		float attenuationLinear = 0.09f;
		float attenuationQuadratic = 0.032f;*/

		float m_attenuationConstant = 0.706f;
		float m_attenuationLinear = 0.073f;
		float m_attenuationQuadratic = 0.011f;
	};
}

