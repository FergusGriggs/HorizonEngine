//waterVertexShader.hlsl
//performs a similar function to the default vertex shader but also displaces the vertices over time

#pragma pack_matrix( row_major )

cbuffer constantBuffer : register(b0)
{
    float4x4 modelViewProjectionMatrix;
    float4x4 modelMatrix;
    float gameTime;
    float waveAmplitude;
};

struct VS_INPUT
{
    float3 pos : POSITION;
    float3 normal : NORMAL;
    float3 tangent : TANGENT;
    float3 bitangent : BITANGENT;
    float2 texCoord : TEXCOORD;
};

struct VS_OUTPUT
{
    float4 pos : SV_POSITION;
    float3 normal : NORMAL;
    float2 texCoord : TEXCOORD;
    float3 worldPos : WORLD_POSIITION;
    float3x3 TBNMatrix : TBN_MATRIX;
};

Texture2D noiseTexture : TEXTURE : register(t4);
SamplerState samplerState : SAMPLER : register(s0);

float GetWaterHeightAt(float posX, float posZ)
{
    float value = sin(posX * 1.5f + gameTime * 0.0017f) * 0.05f + sin(posZ * 1.5f + gameTime * 0.0019f) * 0.05f;
    value += sin(-posX * 0.4f + gameTime * 0.0012f) * 0.15f + sin(posZ * 0.5f + gameTime * 0.0013f) * 0.15f;
    value += sin(posX * 0.2f + gameTime * 0.0006f) * 0.5f + sin(-posZ * 0.22f + gameTime * 0.0004f) * 0.45f;
    return value * waveAmplitude;
    //return noiseTexture.Sample(samplerState, float2(posX, posZ), 1);
}

VS_OUTPUT main(VS_INPUT input)
{
    VS_OUTPUT output;
    float offsetMain = GetWaterHeightAt(input.pos.x, input.pos.z);
    float offsetTangent = GetWaterHeightAt(input.pos.x + 0.025f, input.pos.z);
    float offsetBitangent = GetWaterHeightAt(input.pos.x, input.pos.z + 0.025f);
    
    float3 pos = float3(0.0f, offsetMain, 0.0f);
    float3 tangent = normalize(float3(0.025f, offsetTangent, 0.0f) - pos);
    float3 bitangent = normalize(float3(0.0f, offsetBitangent, 0.025f) - pos);
    float3 normal = normalize(cross(bitangent, tangent));
    
    output.pos = mul(float4(input.pos + float3(0.0f, offsetMain, 0.0f), 1.0f), modelViewProjectionMatrix);
    output.texCoord = input.texCoord; //float2(input.texCoord.x, input.texCoord.y * 0.5f); //float2(input.texCoord.x - gameTime * 0.0001f, input.texCoord.y * 0.5f + gameTime * 0.0001f);
    output.normal = normal;//normalize(mul(float4(input.normal, 0.0f), modelMatrix));
    output.worldPos = mul(float4(input.pos, 1.0f), modelMatrix);

    //float3 tangent = normalize(mul(float4(input.tangent, 0.0f), modelMatrix));
    //float3 normal = normalize(mul(float4(input.normal, 0.0f), modelMatrix));
    //tangent = normalize(tangent - dot(tangent, normal) * normal);
    //float3 bitangent = cross(normal, tangent);

    output.TBNMatrix = transpose(float3x3(tangent, bitangent, normal));

    return output;
}