//noLightPixelShader.hlsl
//allows for models to be drawn without the influence of lights, allowing them to be seen in low light

cbuffer constantBuffer : register(b0)
{
    float3 colour;
};

struct PS_INPUT
{
    float4 pos : SV_POSITION;
    float3 normal : NORMAL;
    float2 texCoord : TEXCOORD;
    float3 worldPos : WORLD_POSIITION;
    float3x3 TBNMatrix : TBN_MATRIX;
};

Texture2D diffuseTexture : TEXTURE : register(t0);
SamplerState samplerState : SAMPLER : register(s0);

float4 main(PS_INPUT input) : SV_TARGET
{
    float3 textureColour = diffuseTexture.Sample(samplerState, input.texCoord);
    float dotResult = abs(dot(float3(0.0f, 1.0f, 0.0f), input.normal));
    dotResult = dotResult * 0.5 + 0.5f;
    return float4(textureColour * colour * dotResult, 1.0f);

}