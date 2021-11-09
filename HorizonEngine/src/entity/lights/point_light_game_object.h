
//Child class of Light which also stores attenuation information

#pragma once

#include "light_game_object.h"

namespace hrzn::entity
{
	class PointLightGameObject : public LightGameObject
	{
	public:
		PointLightGameObject();

		void   updateShaderVariables(gfx::ConstantBuffer<gfx::PixelShaderCB>& pixelShaderCB, size_t lightIndex = 0) override;

		float* getAttenuationConstantPtr();
		float* getAttenuationLinearPtr();
		float* getAttenuationQuadraticPtr();

		void setAttenuationConstant(float attenuationConstant);
		void setAttenuationLinear(float attenuationLinear);
		void setAttenuationQuadratic(float attenuationQuadratic);

	protected:
		float m_attenuationConstant;
		float m_attenuationLinear;
		float m_attenuationQuadratic;
	};
}

