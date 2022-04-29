
#include "../shared/standard.hlsli"

SamplerState samplerState : SAMPLER : register(s0);

Texture2D inputTexture : TEXTURE : register(t0);

float4 main(VSPS_TRANSFER input) : SV_TARGET
{
    float3 defaultColour = inputTexture.Sample(samplerState, input.texCoord).rgb;
    
    float avg = (defaultColour.r + defaultColour.g + defaultColour.b) / 3.0f;
          
    return float4(avg, avg, avg, 1.0f);
}
