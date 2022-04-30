#pragma once

#include <DirectXMath.h>

namespace hrzn::gfx
{
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

	struct SceneCB
	{
		int    m_useNormalMapping;
		int    m_useParallaxOcclusionMapping;
		float  m_depthScale;
		int    m_showWorldNormals;

		int    m_showUVs;
		int    m_cullBackNormals;
		int    m_selfShadowing;
		int    m_gammaCorrection;

		int    m_miscToggleA;
		int    m_miscToggleB;
		int    m_SSAO;
		int    m_roughnessMapping;

		int               m_numPointLights;
		int               m_numSpotLights;
		DirectX::XMFLOAT2 m_padding1;

		DirectionalLightCB m_directionalLight;
		PointLightCB       m_pointLights[10];
		SpotLightCB        m_spotLights[20];
	};

	struct CloudsCB
	{
		float             m_cloudCoverage;
		float             m_lightAbsorbtionThroughClouds;
		float             m_lightAbsorbtionTowardsSun;
		float             m_phaseFactor;

		float             m_darknessThreshold;
		float             m_cloudSpeed;
		float             m_cloudHeight;
		int               m_numSteps;

		float             m_stepSize;
		DirectX::XMFLOAT3 m_padding3;
	};

	struct WaterCB
	{
		int               m_waveCount;
		float             m_waveScale;
		float             m_wavePeriod;
		float             m_waveSpeed;

		float             m_waveSeed;
		float             m_waveScaleMultiplier;
		float             m_foamStart;
		float             m_colourChangeStart;

		int               m_iscolateWaveNum;
		DirectX::XMFLOAT3 m_padding;
	};

	struct AtmosphericCB
	{
		float m_sunSize;
		float m_density;
		float m_multiScatterPhase;
		float m_anisotropicIntensity;

		float m_zenithOffset;
		float m_nightDensity;
		float m_nightZenithYClamp;
		float m_padding1;
	};

	struct AmbientOcclusionCB
	{
		DirectX::XMFLOAT4 m_randomSamples[64];

		int               m_kernelSize;
		float             m_radius;
		float             m_bias;
		float             m_padding;

		DirectX::XMFLOAT2 m_noiseScale;
		DirectX::XMFLOAT2 m_padding2;
	};

	struct NoiseTextureCB
	{
		float m_size;
		float m_height;
		float m_seed;
		float m_noiseSize;
	};

	struct PerFrameCB
	{
		float             m_gameTime;
		DirectX::XMFLOAT3 m_padding1;
	};

	struct PerPassCB
	{
		DirectX::XMFLOAT3 m_cameraPosition;
		float             m_padding1;

		DirectX::XMMATRIX m_viewProjectionMatrix;

		float             m_nearPlane;
		float             m_farPlane;
		DirectX::XMFLOAT2 m_padding2;
	};

	struct PerMaterialCB
	{
		DirectX::XMFLOAT4 m_colour;
	};

	struct PerObjectCB
	{
		DirectX::XMMATRIX m_modelMatrix;
	};

#define MAX_SKINNED_MODEL_BONES 256

	struct PerSkinnedObjectCB
	{
		DirectX::XMMATRIX m_boneTransforms[MAX_SKINNED_MODEL_BONES];
	};
}
