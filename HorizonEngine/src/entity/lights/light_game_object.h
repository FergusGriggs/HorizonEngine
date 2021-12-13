
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

		bool initialize();
		void virtual updateShaderVariables(gfx::ConstantBuffer<gfx::SceneCB>& sceneCB, size_t lightIndex = 0);

		void setColour(DirectX::XMFLOAT3 colour);
		DirectX::XMFLOAT3 getColour();

		void setAmbientStrength(float ambientStrength);
		float getAmbientStrength();


		DirectX::XMFLOAT3 m_colour;

	private:
		float m_ambientStrength;
	};
}
