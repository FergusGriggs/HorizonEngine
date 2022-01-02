
#ifndef __SCENE_HEADER_HLSL__
#define __SCENE_HEADER_HLSL__

struct DirectionalLight
{
    float3 colour;
    float ambientStrength;

    float3 direction;
    float padding1;
};

struct PointLight
{
    float3 position;
    float padding1;

    float3 colour;
    float padding2;

    float attenuationConstant;
    float attenuationLinear;
    float attenuationQuadratic;
    float padding3;
};

struct SpotLight
{
    float3 position;
    float padding1;

    float3 colour;
    float padding2;

    float attenuationConstant;
    float attenuationLinear;
    float attenuationQuadratic;
    float innerCutoff;

    float outerCutoff;
    float3 direction;
};

cbuffer SceneCB : register(b0)
{
	int    cb_useNormalMapping;
	int    cb_useParallaxOcclusionMapping;
	float  cb_depthScale;
	int    cb_showWorldNormals;

	int    cb_showUVs;
	int    cb_cullBackNormals;
	int    cb_selfShadowing;
	int    cb_gammaCorrection;
           
	int    cb_miscToggleA;
	int    cb_miscToggleB;
	int    cb_SSAO;
	int    cb_roughnessMapping;
           
	int    cb_numPointLights;
	int    cb_numSpotLights;
	float2 cb_scenePadding1;

	DirectionalLight cb_directionalLight;
	PointLight       cb_pointLights[10];
	SpotLight        cb_spotLights[20];
}

#endif //__SCENE_HEADER_HLSL__