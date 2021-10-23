
//Function implementations for the Light class

#include "light_game_object.h"

namespace hrzn::entity
{
	LightGameObject::LightGameObject() :
		RenderableGameObject(),
		m_colour(DirectX::XMFLOAT3(1.0f, 1.0f, 1.0f)),
		m_ambientStrength(0.1f)
	{
		m_type = GameObjectType::eLight;
	}

	bool LightGameObject::initialize(std::string label, ID3D11Device* device, ID3D11DeviceContext* deviceContext)
	{
		m_label = label;

		m_model = gfx::ResourceManager::it().getModelPtr("res/models/light.fbx");
		if (m_model == nullptr)
			return false;

		m_transform.setPosition(XMVectorSet(0.0f, 0.0f, 0.0f, 1.0f));
		m_transform.setOrientationQuaternion(XMQuaternionIdentity());

		return true;
	}

	void LightGameObject::updateShaderVariables(gfx::ConstantBuffer<gfx::PixelShaderCB>& cb_ps_pixelShader, int lightIndex)
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
