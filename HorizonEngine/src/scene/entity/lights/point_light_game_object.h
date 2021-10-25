
//Child class of Light which also stores attenuation information

#pragma once

#include "light_game_object.h"

namespace hrzn::scene::entity
{
	class PointLightGameObject : public LightGameObject
	{
	public:
		PointLightGameObject();

		void   updateShaderVariables(gfx::ConstantBuffer<gfx::PixelShaderCB>& pixelShaderCB, int lightIndex = 0) override;
		float* getAttenuationConstantPtr();
		float* getAttenuationLinearPtr();
		float* getAttenuationQuadraticPtr();

		/*float attenuationConstant = 1.0f;
		float attenuationLinear = 0.09f;
		float attenuationQuadratic = 0.032f;*/

		float m_attenuationConstant;
		float m_attenuationLinear;
		float m_attenuationQuadratic;
	};
}

