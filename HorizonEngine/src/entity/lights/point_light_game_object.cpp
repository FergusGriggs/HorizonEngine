
//Function implementation for the PointLight class

#include "point_light_game_object.h"

namespace hrzn::entity
{
	PointLightGameObject::PointLightGameObject() :
		LightGameObject(),
		
		m_attenuationConstant(0.269f),
		m_attenuationLinear(0.018f),
		m_attenuationQuadratic(0.001f)

	{
		m_type = GameObject::Type::ePointLight;
	}

	void PointLightGameObject::updateShaderVariables(gfx::ConstantBuffer<gfx::PixelShaderCB>& pixelShaderCB, size_t lightIndex)
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

	void PointLightGameObject::setAttenuationConstant(float attenuationConstant)
	{
		m_attenuationConstant = attenuationConstant;
	}

	void PointLightGameObject::setAttenuationLinear(float attenuationLinear)
	{
		m_attenuationLinear = attenuationLinear;
	}

	void PointLightGameObject::setAttenuationQuadratic(float attenuationQuadratic)
	{
		m_attenuationQuadratic = attenuationQuadratic;
	}
}