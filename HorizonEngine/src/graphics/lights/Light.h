//Light.h
//Base class for all light objects (Child of RenderableGameObject), stores variables and provides methods that all light objects will use.

#pragma once

#include "../Camera.h"
#include "../RenderableGameObject.h"

class Light : public RenderableGameObject
{
public:
	Light();
	bool Initialize(std::string label, ID3D11Device* device, ID3D11DeviceContext* deviceContext, ResourceManager* resourceManager);
	void virtual UpdateShaderVariables(ConstantBuffer<CB_PS_pixelShader>& cb_ps_pixelShader, int lightIndex = 0);

	void SetColour(DirectX::XMFLOAT3 colour);
	DirectX::XMFLOAT3 GetColour();

	void SetAmbientStrength(float ambientStrength);
	float GetAmbientStrength();


	DirectX::XMFLOAT3 colour = DirectX::XMFLOAT3(1.0f, 1.0f, 1.0f);
private:
	float ambientStrength = 0.3f;
};

