//waterVertexShader.hlsl
//performs a similar function to the default vertex shader but also displaces the vertices over time

#pragma pack_matrix( row_major )

cbuffer constantBuffer : register(b0)
{
    float4x4 modelViewProjectionMatrix;
    float4x4 modelMatrix;

    float gameTime;
    int waveCount;
    float waveScale;
    float wavePeriod;

    float waveSeed;
    float waveSpeed;
    float waveScaleMultiplier;
    int iscolateWaveNum;
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
    float3 baseWorldPos : BASE_WORLD_POSIITION;
};

Texture2D noiseTexture : TEXTURE : register(t4);
SamplerState samplerState : SAMPLER : register(s0);

float hash11(float p)
{
    p = frac(p * 0.1031f);
    p *= p + 33.33f;
    p *= p + p;
    return frac(p);
}

static float3 windDir = float3(1.0f, 0.0f, 0.0f);

VS_OUTPUT main(VS_INPUT input)
{
    VS_OUTPUT output;
    float3 baseFlatWorldPos = mul(float4(input.pos, 1.0f), modelMatrix).xyz;
    //float3 tangentFlatWorldPos = float3(baseFlatWorldPos.x + 0.025f, baseFlatWorldPos.y, baseFlatWorldPos.z);
    //float3 bitangentFlatWorldPos = float3(baseFlatWorldPos.x, baseFlatWorldPos.y, baseFlatWorldPos.z + 0.025f);

    float3 mainFourierOffset = getFourierOffset(baseFlatWorldPos);
    //float3 tangentFourierOffset = getFourierOffset(tangentFlatWorldPos);
    //float3 bitangentFourierOffset = getFourierOffset(bitangentFlatWorldPos);

    //float3 mainFourierPosition = baseFlatWorldPos + mainFourierOffset;
    //float3 tangentFourierPosition = tangentFlatWorldPos + tangentFourierOffset;
    //float3 bitangentFourierPosition = bitangentFlatWorldPos + bitangentFourierOffset;

    //float3 tangent = normalize(tangentFourierPosition - mainFourierPosition);
    //float3 bitangent = normalize(bitangentFourierPosition - mainFourierPosition);
    //float3 normal = normalize(cross(bitangent, tangent));

    output.pos = mul(float4(input.pos + mainFourierOffset, 1.0f), modelViewProjectionMatrix);
    //output.texCoord = float2(input.texCoord.x, input.texCoord.y * 0.75f); //float2(input.texCoord.x - gameTime * 0.0001f, input.texCoord.y * 0.5f + gameTime * 0.0001f);
    //output.normal = normal;
    output.baseWorldPos = baseFlatWorldPos;

    //float3 tangent = normalize(mul(float4(input.tangent, 0.0f), modelMatrix));
    //float3 normal = normalize(mul(float4(input.normal, 0.0f), modelMatrix));
    //tangent = normalize(tangent - dot(tangent, normal) * normal);
    //float3 bitangent = cross(normal, tangent);

    //output.TBNMatrix = transpose(float3x3(tangent, bitangent, normal));

    return output;
}