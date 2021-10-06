
//Function implementation for the SpotLight class

#include "spot_light_game_object.h"

namespace hrzn::entity
{
	SpotLightGameObject::SpotLightGameObject() :
		PointLightGameObject(),

		m_innerCutoff(45.0f),
		m_outerCutoff(35.0f)
	{
		m_type = GameObjectType::eSpotLight;
	}

	void SpotLightGameObject::updateShaderVariables(gfx::ConstantBuffer<gfx::CB_PS_pixelShader>& cb_ps_pixelShader, int lightIndex)
	{
		//NORMALIZE
		XMVECTOR colour = XMLoadFloat3(&m_colour);
		XMStoreFloat3(&cb_ps_pixelShader.m_data.m_spotLights[lightIndex].m_colour, colour);

		cb_ps_pixelShader.m_data.m_spotLights[lightIndex].m_attenuationConstant = m_attenuationConstant;
		cb_ps_pixelShader.m_data.m_spotLights[lightIndex].m_attenuationLinear = m_attenuationLinear;
		cb_ps_pixelShader.m_data.m_spotLights[lightIndex].m_attenuationQuadratic = m_attenuationQuadratic;

		cb_ps_pixelShader.m_data.m_spotLights[lightIndex].m_position = m_transform.getPositionFloat3();
		XMStoreFloat3(&cb_ps_pixelShader.m_data.m_spotLights[lightIndex].m_direction, m_transform.getFrontVector());

		cb_ps_pixelShader.m_data.m_spotLights[lightIndex].m_innerCutoff = m_innerCutoff;
		cb_ps_pixelShader.m_data.m_spotLights[lightIndex].m_outerCutoff = m_outerCutoff;
	}

	float* SpotLightGameObject::getInnerCutoffPtr()
	{
		return &m_innerCutoff;
	}

	float* SpotLightGameObject::getOuterCutoffPtr()
	{
		return &m_outerCutoff;
	}

	float SpotLightGameObject::getInnerCutoff()
	{
		return m_innerCutoff;
	}

	float SpotLightGameObject::getOuterCutoff()
	{
		return m_outerCutoff;
	}

	void SpotLightGameObject::setInnerCutoff(float innerCutoff)
	{
		m_innerCutoff = innerCutoff;
	}

	void SpotLightGameObject::setOuterCutoff(float outerCutoff)
	{
		m_outerCutoff = outerCutoff;
	}
}
