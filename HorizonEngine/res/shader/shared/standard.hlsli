
#ifndef __STANDARD_HEADER_HLSL__
#define __STANDARD_HEADER_HLSL__

#pragma pack_matrix( row_major )

cbuffer PerFrameCB : register(b2)
{
	float  cb_gameTime;
	float3 cb_perFramePadding1;
}

cbuffer PerPassCB : register(b4)
{
    float3 cb_cameraPosition;
    float cb_perPassPadding1;

    float4x4 cb_viewProjectionMatrix;
	
    float cb_nearPlane;
    float cb_farPlane;
    float2 cb_perPassPadding2;
}

cbuffer PerMaterialCB : register(b6)
{
    float4 cb_colour;
}

cbuffer PerObjectCB : register(b8)
{
	float4x4 cb_modelMatrix;
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
