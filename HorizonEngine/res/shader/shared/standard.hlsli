
#ifndef __STANDARD_HEADER_HLSL__
#define __STANDARD_HEADER_HLSL__

cbuffer PerFrameCB : register(b4)
{
	float  cb_gameTime;
	float3 cb_perFramePadding1;
}

cbuffer PerPassCB : register(b8)
{
	float3 cb_cameraPosition;
	float  cb_perPassPadding1;
}

cbuffer PerObjectCB : register(b12)
{
	float4x4 cb_modelMatrix;
	float4x4 cb_modelViewProjectionMatrix;
}

struct VS_INPUT
{
	float3 pos : POSITION;
	float3 normal : NORMAL;
	float3 tangent : TANGENT;
	float3 bitangent : BITANGENT;
	float2 texCoord : TEXCOORD;
};

struct VSPS_TRANSFER
{
	float4 pos : SV_POSITION;
	float3 normal : NORMAL;
	float2 texCoord : TEXCOORD;
	float3 worldPos : WORLD_POSIITION;

	float3 tangent : TANGENT;
	float3 bitangent : BITANGENT;
};

#endif //__STANDARD_HEADER_HLSL__
