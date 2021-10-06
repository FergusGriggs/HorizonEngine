
//Function implementation for the PointLight class

#include "point_light_game_object.h"

namespace hrzn::entity
{
	PointLightGameObject::PointLightGameObject() :
		LightGameObject(),
		
		m_attenuationConstant(0.706f),
		m_attenuationLinear(0.073f),
		m_attenuationQuadratic(0.011f)

	{
		m_type = GameObjectType::ePointLight;
	}

	void PointLightGameObject::updateShaderVariables(gfx::ConstantBuffer<gfx::CB_PS_pixelShader>& cb_ps_pixelShader, int lightIndex)
	{
		cb_ps_pixelShader.m_data.m_pointLights[lightIndex].m_colour = m_colour;

		cb_ps_pixelShader.m_data.m_pointLights[lightIndex].m_attenuationConstant = m_attenuationConstant;
		cb_ps_pixelShader.m_data.m_pointLights[lightIndex].m_attenuationLinear = m_attenuationLinear;
		cb_ps_pixelShader.m_data.m_pointLights[lightIndex].m_attenuationQuadratic = m_attenuationQuadratic;

		cb_ps_pixelShader.m_data.m_pointLights[lightIndex].m_position = m_transform.getPositionFloat3();
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