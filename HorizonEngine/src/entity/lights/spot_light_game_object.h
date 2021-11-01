
//Child class of Light which also stores attenuation information and cutoff values so a feathered cone of light can be modelled

#pragma once

#include "point_light_game_object.h"

namespace hrzn::entity
{
	class SpotLightGameObject : public PointLightGameObject
	{
	public:
		SpotLightGameObject();

		virtual void updateShaderVariables(gfx::ConstantBuffer<gfx::PixelShaderCB>& pixelShaderCB, int lightIndex = 0) override;

		float* getInnerCutoffPtr();
		float* getOuterCutoffPtr();

		float  getInnerCutoff();
		float  getOuterCutoff();

		void   setInnerCutoff(float innerCutoff);
		void   setOuterCutoff(float outerCutoff);

		float  m_innerCutoff;
		float  m_outerCutoff;
	};
}
