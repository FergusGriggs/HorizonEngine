#pragma once

#include <DirectXMath.h>

namespace hrzn::gfx
{
	struct CB_VS_vertexShader
	{
		DirectX::XMMATRIX m_modelViewProjectionMatrix;
		DirectX::XMMATRIX m_modelMatrix;
		float             m_gameTime;
		float             m_waveAmplitude;
	};

	struct CB_DirectionalLight
	{
		DirectX::XMFLOAT3 m_colour;
		float             m_ambientStrength;

		DirectX::XMFLOAT3 m_direction;
		float             m_padding1;
	};

	struct CB_PointLight
	{
		DirectX::XMFLOAT3 m_position;
		float             m_padding1;

		DirectX::XMFLOAT3 m_colour;
		float             m_padding2;

		float             m_attenuationConstant;
		float             m_attenuationLinear;
		float             m_attenuationQuadratic;
		float             m_padding3;
	};

	struct CB_SpotLight
	{
		DirectX::XMFLOAT3 m_position;
		float             m_padding1;

		DirectX::XMFLOAT3 m_colour;
		float             m_padding2;

		float             m_attenuationConstant;
		float             m_attenuationLinear;
		float             m_attenuationQuadratic;
		float             m_innerCutoff;

		float             m_outerCutoff;
		DirectX::XMFLOAT3 m_direction;
	};

	struct CB_Material
	{
		float             m_shininess;
		float             m_specularity;
		DirectX::XMFLOAT2 m_padding1;
	};

	struct CB_PS_pixelShader
	{
		//PACK_SEAM
		int                 m_numPointLights;
		int                 m_numSpotLights;
		DirectX::XMFLOAT2   m_padding1;

		//PACK_SEAM
		int                 m_useNormalMapping;
		int                 m_useParallaxOcclusionMapping;
		float               m_parallaxOcclusionMappingHeight;
		int                 m_fresnel;

		//PACK_SEAM
		CB_Material         m_objectMaterial;

		//PACK_SEAM
		CB_DirectionalLight m_directionalLight;
		CB_PointLight       m_pointLights[10];
		CB_SpotLight        m_spotLights[20];

		//PACK_SEAM
		DirectX::XMFLOAT3   m_cameraPosition;
		float               m_padding2;
	};

	struct CB_PS_noLightPixelShader
	{
		DirectX::XMFLOAT3 m_colour;
		int               m_justColour;
	};

	struct CB_PS_cloudsPixelShader
	{
		DirectX::XMFLOAT3 m_cameraPosition;
		float             m_padding1;

		DirectX::XMFLOAT3 m_lightDirection;
		float             m_padding2;
	};
}