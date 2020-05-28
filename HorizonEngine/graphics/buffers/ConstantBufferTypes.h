#pragma once

#include <DirectXMath.h>

struct CB_VS_vertexShader
{
	DirectX::XMMATRIX modelViewProjectionMatrix;
	DirectX::XMMATRIX modelMatrix;
	float gameTime;
	float waveAmplitude;
};

struct CB_DirectionalLight
{
	DirectX::XMFLOAT3 colour;
	float ambientStrength;

	DirectX::XMFLOAT3 direction;
	float padding1;
};

struct CB_PointLight
{
	DirectX::XMFLOAT3 position;
	float padding1;

	DirectX::XMFLOAT3 colour;
	float padding2;

	float attenuationConstant;
	float attenuationLinear;
	float attenuationQuadratic;
	float padding3;
};

struct CB_SpotLight
{
	DirectX::XMFLOAT3 position;
	float padding1;

	DirectX::XMFLOAT3 colour;
	float padding2;

	float attenuationConstant;
	float attenuationLinear;
	float attenuationQuadratic;
	float innerCutoff;

	float outerCutoff;
	DirectX::XMFLOAT3 direction;
};

struct CB_Material
{
	float shininess;
	float specularity;
	DirectX::XMFLOAT2 padding1;
};

struct CB_PS_pixelShader
{
	//PACK_SEAM
	int numPointLights;
	int numSpotLights;
	DirectX::XMFLOAT2 padding1;

	//PACK_SEAM
	int useNormalMapping;
	int useParallaxOcclusionMapping;
	float parallaxOcclusionMappingHeight;
	int fresnel;

	//PACK_SEAM
	CB_Material objectMaterial;

	//PACK_SEAM
	CB_DirectionalLight directionalLight;
	CB_PointLight pointLights[10];
	CB_SpotLight spotLights[20];

	//PACK_SEAM
	DirectX::XMFLOAT3 cameraPosition;
	float padding2;
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