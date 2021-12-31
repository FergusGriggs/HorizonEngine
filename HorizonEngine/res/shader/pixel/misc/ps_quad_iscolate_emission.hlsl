
#include "../../shared/standard.hlsli"

SamplerState samplerState : SAMPLER : register(s0);

Texture2D inputTexture : TEXTURE : register(t0);

float4 main(VSPS_TRANSFER input) : SV_TARGET
{
    float3 inputSample = inputTexture.Sample(samplerState, input.texCoord).rgb;
    float alpha = 0.0f;
    if (inputSample.r > 0.0f || inputSample.g > 0.0f || inputSample.b > 0.0f)
    {
        alpha = 1.0f;
    }
    return float4(inputSample, alpha);
}