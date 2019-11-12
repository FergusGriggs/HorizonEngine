#pragma once

#include <DirectXMath.h>

struct CB_VS_vertexShader
{
	DirectX::XMMATRIX modelViewProjectionMatrix;
	DirectX::XMMATRIX modelMatrix;
	float gameTime;
};

struct CB_PS_pixelShader
{
	//PACK_SEAM // DIRECTIONAL LIGHT
	DirectX::XMFLOAT3 directionalLightColour;
	float directionalLightAmbientStrength;

	//PACK_SEAM
	float directionalLightSpecularStrength;
	DirectX::XMFLOAT3 directionalLightDirection;

	//PACK_SEAM
	float directionalLightShininess;
	DirectX::XMFLOAT3 padding;

	//PACK_SEAM // POINT LIGHT
	DirectX::XMFLOAT3 pointLightColour;
	float pointLightAmbientStrength;

	//PACK_SEAM
	float pointLightAttenuationConstant;
	float pointLightAttenuationLinear;
	float pointLightAttenuationQuadratic;
	float pointLightSpecularStrength;

	//PACK_SEAM
	DirectX::XMFLOAT3 pointLightPosition;
	float pointLightShininess;

	//PACK_SEAM // SPOT LIGHT
	DirectX::XMFLOAT3 spotLightColour;
	float spotLightAmbientStrength;

	//PACK_SEAM
	float spotLightAttenuationConstant;
	float spotLightAttenuationLinear;
	float spotLightAttenuationQuadratic;
	float spotLightSpecularStrength;

	//PACK_SEAM
	DirectX::XMFLOAT3 spotLightPosition;
	float spotLightShininess;

	//PACK_SEAM
	DirectX::XMFLOAT3 spotLightDirection;
	float spotLightInnerCutoff;

	//PACK_SEAM
	float spotLightOuterCutoff;
	DirectX::XMFLOAT3 cameraPosition;

	//PACK_SEAM
	int useNormalMapping;
	int useParallaxOcclusionMapping;
	float parallaxOcclusionMappingHeight;
};

struct CB_PS_noLightPixelShader
{
	DirectX::XMFLOAT3 colour;
};
