
#include "../../shared/standard.hlsli"

SamplerState samplerState : SAMPLER : register(s0);

Texture2D inputTexture : TEXTURE : register(t0);

static const float weights[5] = { 0.2270270270, 0.1945945946, 0.1216216216, 0.0540540541, 0.0162162162 };

float4 main(VSPS_TRANSFER input) : SV_TARGET
{
    float2 imageSize;
    inputTexture.GetDimensions(imageSize.x, imageSize.y);
    
    float2 texOffset = 1.0f / imageSize;
    float3 result = inputTexture.Sample(samplerState, input.texCoord).rgb * weights[0];
    for (int i = 1; i < 5; ++i)
    {
        result += inputTexture.Sample(samplerState, input.texCoord + float2(0.0f, texOffset.y * i)).rgb * weights[i];
        result += inputTexture.Sample(samplerState, input.texCoord - float2(0.0f, texOffset.y * i)).rgb * weights[i];
    }
    return float4(result, 1.0f);
}