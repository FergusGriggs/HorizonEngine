//vertexShader.hlsl
//Default vertex shader

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

    float3 tangent : TANGENT;
    float3 bitangent : BITANGENT;

    //float3x3 TBNMatrix : TBN_MATRIX;
};

VS_OUTPUT main (VS_INPUT input)
{
    VS_OUTPUT output;
    output.pos = mul(float4(input.pos, 1.0f), modelViewProjectionMatrix);
    output.texCoord = input.texCoord;
    output.normal = normalize(mul(float4(input.normal, 0.0f), modelMatrix));
    output.worldPos = mul(float4(input.pos, 1.0f), modelMatrix);

    output.tangent = normalize(mul(float4(input.tangent, 0.0f), modelMatrix));
    output.bitangent = normalize(mul(float4(input.bitangent, 0.0f), modelMatrix));

    //float3 bitangent = normalize(mul(float4(input.bitangent, 0.0f), modelMatrix));

    // re-orthogonalize T with respect to N
    //output.tangent = normalize(output.tangent - dot(output.tangent, output.normal) * output.normal);
    // then retrieve perpendicular vector B with the cross product of T and N
    //output.bitangent = -normalize(cross(output.tangent, output.normal));

    //output.TBNMatrix = float3x3(tangent, bitangent, normal);

    //output.TBNMatrix = transpose(float3x3(tangent, bitangent, output.normal));

    return output;
}