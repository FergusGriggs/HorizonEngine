
#include "../../shared/standard.hlsli"

SamplerState samplerState : SAMPLER : register(s0);

Texture2D normalTexture : TEXTURE : register(t0);
Texture2D blurredTexture : TEXTURE : register(t1);
Texture2D depthTexture : TEXTURE : register(t2);

static const float sc_focusDepth = 0.35f;

float4 main(VSPS_TRANSFER input) : SV_TARGET
{
    float3 normalColour = normalTexture.Sample(samplerState, input.texCoord).rgb;
    float3 blurredColour = blurredTexture.Sample(samplerState, input.texCoord).rgb;
    float depth = pow(abs(depthTexture.Sample(samplerState, input.texCoord).r), 40.0f);
    
    float depthDiff = min(1.0f, abs(sc_focusDepth - depth) * 2.0f);
    
    float3 finalColour = lerp(normalColour, blurredColour, depthDiff);
    
    return float4(finalColour, 1.0f);
}