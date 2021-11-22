//noLightPixelShader.hlsl
//allows for models to be drawn without the influence of lights, allowing them to be seen in low light

cbuffer constantBuffer : register(b0)
{
    float3 colour;
    int justColour;

    float3 cameraPos;
    float padding1;
};

struct PS_INPUT
{
    float4 pos : SV_POSITION;
    float3 normal : NORMAL;
    float2 texCoord : TEXCOORD;
    float3 worldPos : WORLD_POSIITION;

    float3 tangent : TANGENT;
    float3 bitangent : BITANGENT;
    //float3x3 TBNMatrix : TBN_MATRIX;
};

Texture2D diffuseTexture : TEXTURE : register(t0);
SamplerState samplerState : SAMPLER : register(s0);

float4 main(PS_INPUT input) : SV_TARGET
{
    float3 textureColour = diffuseTexture.Sample(samplerState, input.texCoord).rgb;
    
    if (justColour)
    {
        return float4(textureColour.r, textureColour.g, textureColour.b, 1.0f);
    }
    
    float dotResult = abs(dot(float3(0.0f, 1.0f, 0.0f), input.normal));
    dotResult = dotResult * 0.4 + 0.6f;

    float3 viewDirection = normalize(input.worldPos - cameraPos);
    //float fresnelFactor = clamp(1.0f - dot(-input.normal, viewDirection), 0.0f, 1.0f);
    //fresnelFactor = pow(fresnelFactor, 5.0f) * 0.5f;

    float facingFactor = pow(max(0.0f, dot(-input.normal, viewDirection)), 2.0f) * 0.25;

    return float4(textureColour * colour * dotResult + facingFactor, 1.0f);

}