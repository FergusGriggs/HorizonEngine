
#include "../shared/standard.hlsli"

SamplerState samplerState : SAMPLER : register(s0);

Texture2D colourTexture : TEXTURE : register(t0);
Texture2D blurredEmission : TEXTURE : register(t1);

static const float sc_focusDepth = 0.25f;

float4 main(VSPS_TRANSFER input) : SV_TARGET
{
    float3 colour = colourTexture.Sample(samplerState, input.texCoord).rgb;
    float3 blurredEmissionColour = blurredEmission.Sample(samplerState, input.texCoord).rgb;
    
    return float4(colour + blurredEmissionColour, 1.0f);
}