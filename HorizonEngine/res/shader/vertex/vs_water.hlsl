
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
    //float3 tangentFlatWorldPos = float3(baseFlatWorldPos.x + 0.025f, baseFlatWorldPos.y, baseFlatWorldPos.z);
    //float3 bitangentFlatWorldPos = float3(baseFlatWorldPos.x, baseFlatWorldPos.y, baseFlatWorldPos.z + 0.025f);

    float3 mainFourierOffset = getFourierOffset(baseFlatWorldPos);
    //float3 tangentFourierOffset = getFourierOffset(tangentFlatWorldPos);
    //float3 bitangentFourierOffset = getFourierOffset(bitangentFlatWorldPos);

    //float3 mainFourierPosition = baseFlatWorldPos + mainFourierOffset;
    //float3 tangentFourierPosition = tangentFlatWorldPos + tangentFourierOffset;
    //float3 bitangentFourierPosition = bitangentFlatWorldPos + bitangentFourierOffset;

    //float3 tangent = normalize(tangentFourierPosition - mainFourierPosition);
    //float3 bitangent = normalize(bitangentFourierPosition - mainFourierPosition);
    //float3 normal = normalize(cross(bitangent, tangent));

    output.pos = mul(float4(input.pos + mainFourierOffset, 1.0f), mul(cb_modelMatrix, cb_viewProjectionMatrix));
    //output.texCoord = float2(input.texCoord.x, input.texCoord.y * 0.75f); //float2(input.texCoord.x - gameTime * 0.0001f, input.texCoord.y * 0.5f + gameTime * 0.0001f);
    //output.normal = normal;
    output.baseWorldPos = baseFlatWorldPos;

    //float3 tangent = normalize(mul(float4(input.tangent, 0.0f), modelMatrix));
    //float3 normal = normalize(mul(float4(input.normal, 0.0f), modelMatrix));
    //tangent = normalize(tangent - dot(tangent, normal) * normal);
    //float3 bitangent = cross(normal, tangent);

    //output.TBNMatrix = transpose(float3x3(tangent, bitangent, normal));

    return output;
}
