
//Base class for all light objects (Child of RenderableGameObject), stores variables and provides methods that all light objects will use.

#pragma once

#include "../camera_game_object.h"
#include "../renderable_game_object.h"

namespace hrzn::entity
{
	class LightGameObject : public RenderableGameObject
	{
	public:
		LightGameObject();

		bool initialize(std::string label);
		void virtual updateShaderVariables(gfx::ConstantBuffer<gfx::PixelShaderCB>& cb_ps_pixelShader, int lightIndex = 0);

		void setColour(DirectX::XMFLOAT3 colour);
		DirectX::XMFLOAT3 getColour();

		void setAmbientStrength(float ambientStrength);
		float getAmbientStrength();


		DirectX::XMFLOAT3 m_colour = DirectX::XMFLOAT3(1.0f, 1.0f, 1.0f);

	private:
		float m_ambientStrength = 0.3f;
	};
}
