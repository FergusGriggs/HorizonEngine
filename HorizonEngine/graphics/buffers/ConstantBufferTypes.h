#pragma once

#include <DirectXMath.h>

struct CB_VS_vertexShader
{
	DirectX::XMMATRIX modelViewProjectionMatrix;
	DirectX::XMMATRIX modelMatrix;
	float gameTime;
	float waveAmplitude;
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
	int fresnel;
};

struct CB_PS_noLightPixelShader
{
	DirectX::XMFLOAT3 colour;
	int justColour;
};

struct CB_PS_cloudsPixelShader
{
	DirectX::XMFLOAT3 cameraPosition;
	float padding1;

	DirectX::XMFLOAT3 lightDirection;
	float padding2;
};

