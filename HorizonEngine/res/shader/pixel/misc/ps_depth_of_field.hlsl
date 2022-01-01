
#include "../../shared/standard.hlsli"

SamplerState samplerState : SAMPLER : register(s0);

Texture2D normalTexture : TEXTURE : register(t0);
Texture2D blurredTexture : TEXTURE : register(t1);
Texture2D depthTexture : TEXTURE : register(t2);

static const float sc_focusDepth = 0.3f;

float getLinearProgress(float edge0, float edge1, float x)
{
    return clamp((x - edge0) / (edge1 - edge0), 0.0f, 1.0f);
}

float4 main(VSPS_TRANSFER input) : SV_TARGET
{
    float3 normalColour = normalTexture.Sample(samplerState, input.texCoord).rgb;
    float3 blurredColour = blurredTexture.Sample(samplerState, input.texCoord).rgb;
    float depth = pow(abs(depthTexture.Sample(samplerState, input.texCoord).r), 40.0f);
    
    float depthDiff = pow(getLinearProgress(0.1f, 0.25f, abs(sc_focusDepth - depth)), 2.0f);
    
    float3 finalColour = lerp(normalColour, blurredColour, depthDiff);
    
    return float4(finalColour, 1.0f);
}