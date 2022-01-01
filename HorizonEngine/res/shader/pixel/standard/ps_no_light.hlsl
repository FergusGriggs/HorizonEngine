
// Allows for models to be drawn without the influence of lights

#include "../../shared/standard.hlsli"

Texture2D diffuseTexture : TEXTURE : register(t0);
SamplerState samplerState : SAMPLER : register(s0);

float4 main(VSPS_TRANSFER input) : SV_TARGET
{
    float3 textureColour = diffuseTexture.Sample(samplerState, input.texCoord).rgb;
    
    float dotResult = abs(dot(float3(0.0f, 1.0f, 0.0f), input.normal));
    dotResult = dotResult * 0.4 + 0.6f;

    float3 viewDirection = normalize(input.worldPos - cb_cameraPosition);
    float facingFactor = pow(max(0.0f, dot(-input.normal, viewDirection)), 2.0f) * 0.25;

    return float4(textureColour * cb_colour.rgb * dotResult + facingFactor, 1.0f);
}