#pragma pack_matrix( row_major )

#include "../../shared/standard.hlsli"

cbuffer AmbientOcclusionCB : register(b1)
{
    float4 randomSamples[64];

    int   kernelSize;
    float radius;
    float bias;
    float padding;

    float2 noiseScale;
    float2 padding2;
};

Texture2D depthTexture : TEXTURE : register(t0);
Texture2D positionTexture : TEXTURE : register(t1);
Texture2D normalTexture : TEXTURE : register(t2);
Texture2D noiseTexture : TEXTURE : register(t3);

SamplerState samplerState : SAMPLER : register(s0);

float main(VSPS_TRANSFER input) : SV_TARGET
{
    float3 normal = normalTexture.Sample(samplerState, input.texCoord).rgb;
    if (length(normal) < 0.95f)
    {
        return 1.0f;
    }
    float3 position = positionTexture.Sample(samplerState, input.texCoord).rgb;
    float depth = depthTexture.Sample(samplerState, input.texCoord).r;
    
    float3 randomVector = normalize(noiseTexture.Sample(samplerState, input.texCoord * noiseScale).rgb);
    float3 tangent = normalize(randomVector - normal * dot(randomVector, normal));
    float3 bitangent = normalize(cross(normal, tangent));
    float3x3 TBN = float3x3(tangent, bitangent, normal);

    float occlusion = 0.0f;

    for (int i = 0; i < kernelSize; ++i)
    {
        // get sample position
        float3 randomSample = mul(randomSamples[i].xyz, TBN); // from tangent to view-space
        randomSample = position + randomSample * radius;

        // project sample position (to sample texture) (to get position on screen/texture)
        float4 offset = float4(randomSample, 1.0f);
        offset = mul(offset, cb_viewProjectionMatrix); // from view to clip-space
        offset.xy /= offset.w; // perspective divide
        offset.xy = offset.xy * 0.5 + 0.5; // transform to range 0.0 - 1.0
        offset.y = 1.0f - offset.y;

        if (offset.x > 0.0f && offset.x < 1.0f && offset.y > 0.0f && offset.y < 1.0f)
        {
            float sampleDepth = depthTexture.Sample(samplerState, offset.xy).r; // get depth value of kernel sample
            float normalizedSampleDepth = 2.0f * sampleDepth - 1.0f;
            float trueSampleDepth = (2.0f * cb_nearPlane * cb_farPlane) / (cb_farPlane + cb_nearPlane - normalizedSampleDepth * (cb_farPlane - cb_nearPlane));

            float diff = trueSampleDepth - offset.z;

            if (diff < 0.0f)
            {
                occlusion += smoothstep(40.0f, 10.0f, offset.z) * smoothstep(-radius, -radius * 0.5f, diff);
            }
        }
    }

    occlusion = 1.0f - (occlusion / kernelSize);

    return occlusion;
}