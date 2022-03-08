
// Performs a similar function to the default vertex shader but also displaces the vertices over time

#pragma pack_matrix( row_major )

#include "../shared/standard.hlsli"
#include "../shared/water.hlsli"

SamplerState samplerState : SAMPLER : register(s0);

/***********************************************

MARKING SCHEME: Recent / Advanced graphics algorithms or techniques

DESCRIPTION: This is the shader used to move the verticies of the
water mesh in the default scene

COMMENT INDEX: 19

***********************************************/

VSPS_TRANSFER_WATER main(VS_INPUT input)
{
	VSPS_TRANSFER_WATER output;
    float3 baseFlatWorldPos = mul(float4(input.pos, 1.0f), cb_modelMatrix).xyz;
    float3 mainFourierOffset = getFourierOffset(baseFlatWorldPos);

    output.pos = mul(float4(input.pos + mainFourierOffset, 1.0f), mul(cb_modelMatrix, cb_viewProjectionMatrix));
    output.baseWorldPos = baseFlatWorldPos;

    return output;
}
