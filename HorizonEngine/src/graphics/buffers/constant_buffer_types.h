#pragma once

#include <DirectXMath.h>

namespace hrzn::gfx
{
	struct VertexShaderCB
	{
		DirectX::XMMATRIX m_modelViewProjectionMatrix;
		DirectX::XMMATRIX m_modelMatrix;
		float             m_gameTime;
		float             m_padding;
	};

	struct WaterVertexShaderCB
	{
		DirectX::XMMATRIX  m_modelViewProjectionMatrix;
		DirectX::XMMATRIX  m_modelMatrix;

		float              m_gameTime;
		int                m_waveCount; 
		float              m_waveScale;
		float              m_wavePeriod;

		float m_waveSeed;
		float m_waveSpeed;
		float             m_waveScaleMultiplier;
		int               m_iscolateWaveNum;
	};

	struct WaterPixelShaderCB
	{
		DirectX::XMFLOAT3 m_cameraPosition;
		float             m_gameTime;

		//Water parameters
		int   m_waveCount;
		float m_waveScale;
		float m_wavePeriod;
		float m_waveSpeed;

		float m_waveSeed;
		float m_waveScaleMultiplier;
		float m_foamStart;
		float m_colourChangeStart;

		DirectX::XMFLOAT3 m_lightDirection;
		int               m_iscolateWaveNum;

		DirectX::XMFLOAT3 m_lightColour;
		float             m_padding2;
	};

	struct DirectionalLightCB
	{
		DirectX::XMFLOAT3 m_colour;
		float             m_ambientStrength;

		DirectX::XMFLOAT3 m_direction;
		float             m_padding1;
	};

	struct PointLightCB
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

	struct SpotLightCB
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

	struct MaterialCB
	{
		float             m_shininess;
		float             m_specularity;
		DirectX::XMFLOAT2 m_padding1;
	};

	struct PixelShaderCB
	{
		//PACK_SEAM
		int                 m_numPointLights;
		int                 m_numSpotLights;
		DirectX::XMFLOAT2   m_padding1;

		//PACK_SEAM
		int                 m_useNormalMapping;
		int                 m_useParallaxOcclusionMapping;
		float               m_depthScale;
		int                 m_showWorldNormals;

		int                 m_showUVs;
		int                 m_cullBackNormals;
		int                 m_selfShadowing;
		int                 m_gammaCorrection;
		                    
		int                 m_miscToggleA;
		int                 m_miscToggleB;
		int                 m_miscToggleC;
		int                 m_miscToggleD;

		//PACK_SEAM
		MaterialCB          m_objectMaterial;

		//PACK_SEAM
		DirectionalLightCB  m_directionalLight;
		PointLightCB        m_pointLights[10];
		SpotLightCB         m_spotLights[20];

		//PACK_SEAM
		DirectX::XMFLOAT3   m_cameraPosition;
		float               m_padding2;
	};

	struct NoLightPixelShaderCB
	{
		DirectX::XMFLOAT3 m_colour;
		int               m_justColour;
	};

	struct AtmosphericPixelShaderCB
	{
		DirectX::XMFLOAT3 m_sunDirection;
		float             m_sunSize;

		DirectX::XMFLOAT3 m_cameraPosition;
		float             m_padding;

		float  m_density;
		float  m_multiScatterPhase;
		float  m_anisotropicIntensity;
		float  m_zenithOffset;

		float             m_nightDensity;
		float             m_nightZenithYClamp;
		DirectX::XMFLOAT2 m_padding1;
	};

	struct CloudsPixelShaderCB
	{
		DirectX::XMFLOAT3 m_cameraPosition;
		float             m_padding1;

		DirectX::XMFLOAT3 m_lightDirection;
		float             m_cloudCoverage;

		DirectX::XMFLOAT3 m_lightColour;
		float             m_gameTime;

		float             m_lightAbsorbtionThroughClouds;
		float             m_lightAbsorbtionTowardsSun;
		float             m_phaseFactor;
		float             m_darknessThreshold;

		float             m_cloudSpeed;
		float             m_cloudHeight;
		int               m_numSteps;
		float             m_stepSize;
	};

	struct NoiseTextureComputeShaderCB
	{
		float m_size;
		float m_height;
		float m_seed;
		float m_noiseSize;
	};
}