
// Skinned vertex shader

#include "../shared/standard.hlsli"

VSPS_TRANSFER main(VS_SKINNED_INPUT input)
{
    VSPS_TRANSFER output;
    
    float4x4 boneTransform = cb_boneTransforms[input.boneIds[0]] * input.boneWeights[0];
    boneTransform += cb_boneTransforms[input.boneIds[1]] * input.boneWeights[1];
    boneTransform += cb_boneTransforms[input.boneIds[2]] * input.boneWeights[2];
    boneTransform += cb_boneTransforms[input.boneIds[3]] * input.boneWeights[3];
    
    float4x4 finalModelTransform = boneTransform;
    
    output.worldPos = mul(float4(input.pos, 1.0f), finalModelTransform);
    output.pos = mul(float4(output.worldPos, 1.0f), cb_viewProjectionMatrix);
    output.texCoord = input.texCoord;
    output.normal = normalize(mul(float4(input.normal, 0.0f), finalModelTransform).xyz);
    output.tangent = normalize(mul(float4(input.tangent, 0.0f), finalModelTransform).xyz);
    output.bitangent = normalize(mul(float4(input.bitangent, 0.0f), finalModelTransform).xyz);
    
    //float4 position = float4(0.0f, 0.0f, 0.0f, 0.0f);
    //float4 normal = float4(0.0f, 0.0f, 0.0f, 0.0f);
    //float4 tangent = float4(0.0f, 0.0f, 0.0f, 0.0f);
    //float4 bitangent = float4(0.0f, 0.0f, 0.0f, 0.0f);
    
    //for (int i = 0; i < 4; ++i)
    //{
    //    position += mul(float4(input.pos, 1.0f), cb_boneTransforms[input.boneIds[i]]) * input.boneWeights[i];
    //    normal += mul(float4(input.normal, 0.0f), cb_boneTransforms[input.boneIds[i]]) * input.boneWeights[i];
    //    tangent += mul(float4(input.tangent, 0.0f), cb_boneTransforms[input.boneIds[i]]) * input.boneWeights[i];
    //    bitangent += mul(float4(input.bitangent, 0.0f), cb_boneTransforms[input.boneIds[i]]) * input.boneWeights[i];
    //}
    
    //output.worldPos = position.xyz;
    //output.pos = mul(position, cb_viewProjectionMatrix);
    //output.texCoord = input.texCoord;
    
    //output.normal = normalize(normal.xyz);
    //output.tangent = normalize(tangent.xyz);
    //output.bitangent = normalize(bitangent.xyz);
    
    return output;
}