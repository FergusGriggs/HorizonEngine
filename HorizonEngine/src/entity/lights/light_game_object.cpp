
//Function implementations for the Light class

#include "light_game_object.h"

namespace hrzn::entity
{
	LightGameObject::LightGameObject() :
		RenderableGameObject(),
		m_colour(DirectX::XMFLOAT3(1.0f, 1.0f, 1.0f)),
		m_ambientStrength(0.2f)
	{
		m_type = GameObject::Type::eLight;
	}

	bool LightGameObject::initialize()
	{
		m_model = gfx::ResourceManager::it().getModelPtr("res/models/light.obj");
		if (m_model == nullptr) return false;
		
		return true;
	}

	void LightGameObject::updateShaderVariables(gfx::ConstantBuffer<gfx::PixelShaderCB>& cb_ps_pixelShader, size_t lightIndex)
	{
		cb_ps_pixelShader.m_data.m_directionalLight.m_colour = m_colour;
		cb_ps_pixelShader.m_data.m_directionalLight.m_ambientStrength = m_ambientStrength;

		XMStoreFloat3(&cb_ps_pixelShader.m_data.m_directionalLight.m_direction, m_transform.getFrontVector());
	}

	void LightGameObject::setColour(DirectX::XMFLOAT3 colour)
	{
		float maxColourValue = fmaxf(fmaxf(fmaxf(colour.x, colour.y), colour.z), 1.0f);

		m_colour = DirectX::XMFLOAT3(colour.x / maxColourValue, colour.y / maxColourValue, colour.z / maxColourValue);
	}

	DirectX::XMFLOAT3 LightGameObject::getColour()
	{
		return m_colour;
	}

	void LightGameObject::setAmbientStrength(float ambientStrength)
	{
		m_ambientStrength = ambientStrength;
	}

	float LightGameObject::getAmbientStrength()
	{
		return m_ambientStrength;
	}
}
