
//Function implementation for the PointLight class

#include "point_light_game_object.h"

namespace hrzn::scene::entity
{
	PointLightGameObject::PointLightGameObject() :
		LightGameObject(),
		
		m_attenuationConstant(0.269f),
		m_attenuationLinear(0.018f),
		m_attenuationQuadratic(0.001f)

	{
		m_type = GameObjectType::ePointLight;
	}

	void PointLightGameObject::updateShaderVariables(gfx::ConstantBuffer<gfx::PixelShaderCB>& pixelShaderCB, int lightIndex)
	{
		pixelShaderCB.m_data.m_pointLights[lightIndex].m_colour = m_colour;

		pixelShaderCB.m_data.m_pointLights[lightIndex].m_attenuationConstant = m_attenuationConstant;
		pixelShaderCB.m_data.m_pointLights[lightIndex].m_attenuationLinear = m_attenuationLinear;
		pixelShaderCB.m_data.m_pointLights[lightIndex].m_attenuationQuadratic = m_attenuationQuadratic;

		pixelShaderCB.m_data.m_pointLights[lightIndex].m_position = m_transform.getPositionFloat3();
	}

	float* PointLightGameObject::getAttenuationConstantPtr()
	{
		return &m_attenuationConstant;
	}

	float* PointLightGameObject::getAttenuationLinearPtr()
	{
		return &m_attenuationLinear;
	}

	float* PointLightGameObject::getAttenuationQuadraticPtr()
	{
		return &m_attenuationQuadratic;
	}
}