
#include "../shared/standard.hlsli"

SamplerState samplerState : SAMPLER : register(s0);

Texture2D inputTexture : TEXTURE : register(t0);

float4 main(VSPS_TRANSFER input) : SV_TARGET
{
    return inputTexture.Sample(samplerState, input.texCoord);
}