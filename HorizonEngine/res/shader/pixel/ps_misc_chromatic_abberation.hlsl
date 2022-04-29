
#include "../shared/standard.hlsli"

SamplerState samplerState : SAMPLER : register(s0);

Texture2D inputTexture : TEXTURE : register(t0);

float getLinearProgress(float edge0, float edge1, float x)
{
    return clamp((x - edge0) / (edge1 - edge0), 0.0f, 1.0f);
}

float4 main(VSPS_TRANSFER input) : SV_TARGET
{
    float2 toCentre = float2(0.5f, 0.5f) - input.texCoord;
    float distToCentre = length(toCentre) * 2.0f;
    toCentre = normalize(toCentre);
    
    float offsetScalar = pow(abs(getLinearProgress(0.0f, 1.8f, distToCentre)), 3.0f) * 0.04f;
    
    float2 offsetR = offsetScalar * toCentre * 0.1f;
    float2 offsetG = offsetScalar * toCentre * 0.5f;
    float2 offsetB = offsetScalar * toCentre * 1.0f;
    
    float r = inputTexture.Sample(samplerState, input.texCoord + offsetR).r;
    float g = inputTexture.Sample(samplerState, input.texCoord + offsetG).g;
    float b = inputTexture.Sample(samplerState, input.texCoord + offsetB).b;
    
    return float4(r, g, b, 1.0f);
}