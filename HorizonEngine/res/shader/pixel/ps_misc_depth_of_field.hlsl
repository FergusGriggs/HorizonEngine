
#include "../shared/standard.hlsli"

SamplerState samplerState : SAMPLER : register(s0);

Texture2D normalTexture : TEXTURE : register(t0);
Texture2D blurredTexture : TEXTURE : register(t1);
Texture2D depthTexture : TEXTURE : register(t2);

static const float sc_focusDepth = 0.1f;

float getLinearProgress(float edge0, float edge1, float x)
{
    return clamp((x - edge0) / (edge1 - edge0), 0.0f, 1.0f);
}

/***********************************************

MARKING SCHEME: Recent / Advanced graphics algorithms or techniques

DESCRIPTION: This is the shader used to merge the blurred texture and the regular
colour texture using the depth texture and whatever depth is sampled at the centre of the screen

COMMENT INDEX: 16

***********************************************/

float4 main(VSPS_TRANSFER input) : SV_TARGET
{
    float focusDepth = pow(abs(depthTexture.Sample(samplerState, float2(0.5f, 0.5f)).r), 50.0f);
    
    float3 normalColour = normalTexture.Sample(samplerState, input.texCoord).rgb;
    float3 blurredColour = blurredTexture.Sample(samplerState, input.texCoord).rgb;
    float depth = pow(abs(depthTexture.Sample(samplerState, input.texCoord).r), 50.0f);
    
    float startTransition = min(1.0f, 0.1f);
    float endTransition = min(1.0f, 0.25f + focusDepth * 0.5f);
    
    float depthDiff = pow(getLinearProgress(startTransition, endTransition, abs(focusDepth - depth)), 2.0f);
    
    float3 finalColour = lerp(normalColour, blurredColour, depthDiff);
    
    return float4(finalColour, 1.0f);
}