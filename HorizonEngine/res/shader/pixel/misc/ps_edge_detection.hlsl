
#include "../../shared/standard.hlsli"

SamplerState samplerState : SAMPLER : register(s0);

Texture2D colourTexture : TEXTURE : register(t0);
Texture2D depthTexture : TEXTURE : register(t1);
Texture2D normalTexture : TEXTURE : register(t2);

static const float sc_focusDepth = 0.35f;

float getDepthAt(float2 texCoord)
{
    return pow(abs(depthTexture.Sample(samplerState, texCoord).r), 40.0f);
}

bool normalIsValid(float3 normal)
{
    if (length(normal) < 0.95f)
    {
        return false;
    }
    return true;
}

float4 main(VSPS_TRANSFER input) : SV_TARGET
{
    float2 imageSize;
    colourTexture.GetDimensions(imageSize.x, imageSize.y);
    
    float centreDepth = getDepthAt(input.texCoord);
    float3 centreNormal = normalTexture.Sample(samplerState, input.texCoord).rgb;
    
    bool useNormals = false;
    if (normalIsValid(centreNormal))
    {
        useNormals = true;
    }
    
    float2 texOffset = 1.0f / imageSize;
    float edge = 1.0f;
    
    for (int i = -1; i < 2; ++i)
    {
        for (int j = -1; j < 2; ++j)
        {
            float2 offsetTexCoord = input.texCoord + float2(i * texOffset.x, j * texOffset.y);
            if (abs(centreDepth - getDepthAt(offsetTexCoord)) > 0.05)
            {
                edge = 0.0f;
            }
            if (useNormals)
            {
                float3 normal = normalTexture.Sample(samplerState, offsetTexCoord).rgb;
                if (normalIsValid(normal))
                {
                    float dotResult = dot(normal, centreNormal);
                    if (dotResult < 0.8f)
                    {
                        edge = (dotResult - 0.8f) * 5.0f;
                    }
                }
            }
        }
    }
    
    float3 colour = colourTexture.Sample(samplerState, input.texCoord).rgb;
    return float4(colour * edge, 1.0f);
}