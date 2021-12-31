
#include "../../shared/standard.hlsli"

SamplerState samplerState : SAMPLER : register(s0);

Texture2D inputTexture : TEXTURE : register(t0);

float4 main(VSPS_TRANSFER input) : SV_TARGET
{
    float3 defaultColour = inputTexture.Sample(samplerState, input.texCoord).rgb;
    
    float3 sepiaColour;
    sepiaColour.r = dot(defaultColour.rgb, half3(0.393f, 0.769f, 0.189f));
    sepiaColour.g = dot(defaultColour.rgb, half3(0.349f, 0.686f, 0.168f));
    sepiaColour.b = dot(defaultColour.rgb, half3(0.272f, 0.534f, 0.131f));
          
    return float4(sepiaColour, 1.0f);
}