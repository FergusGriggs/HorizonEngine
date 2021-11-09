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
    //float3 normal : NORMAL;
    //float2 texCoord : TEXCOORD;
    float3 baseWorldPos : BASE_WORLD_POSIITION;
    //float3x3 TBNMatrix : TBN_MATRIX;
};

Texture2D noiseTexture : TEXTURE : register(t4);
SamplerState samplerState : SAMPLER : register(s0);

float GetWaterHeightAt(float posX, float posZ)
{
    float value = sin(posX * 1.5f + gameTime * 1.7f) * 0.05f + sin(posZ * 1.5f + gameTime * 1.9f) * 0.05f;
    value += sin(-posX * 0.4f + gameTime * 1.2f) * 0.15f + sin(posZ * 0.5f + gameTime * 1.3f) * 0.15f;
    value += sin(posX * 0.2f + gameTime * 0.6f) * 0.5f + sin(-posZ * 0.22f + gameTime * 0.4f) * 0.45f;
    return value * waveScale;
    //return noiseTexture.Sample(samplerState, float2(posX, posZ), 1);
}

float hash11(float p)
{
    p = frac(p * 0.1031f);
    p *= p + 33.33f;
    p *= p + p;
    return frac(p);
}

static float3 windDir = float3(1.0f, 0.0f, 0.0f);

float3 getFourierOffset(float3 position)
{
    float3 flatPosition = float3(position.x, 0.0f, position.z);
    float3 finalOffset = float3(0.0f, 0.0f, 0.0f);

    float scale = 0.5f;

    int waveNum = 0;

    [unroll(50)]
    while (waveNum < waveCount)
    {
        if (iscolateWaveNum == -1 || iscolateWaveNum == waveNum)
        {
            float waveAngle = (float)waveNum * waveSeed;// hash11((float)waveNum * waveSeed)* waveSeed;
            float3 waveDir = float3(cos(waveAngle), 0.0f, sin(waveAngle));
            //float3 waveDirRight = float3(waveDir.z, 0.0f, -waveDir.x);

            float windScaleModifier = dot(waveDir, windDir) * 0.35f + 0.7f;

            float initialWaveDist = dot(flatPosition, waveDir);
            float distWaveTravelled = gameTime * waveSpeed * ((float)waveNum * 1.0f + 1.0f) * scale + initialWaveDist;

            float angle = distWaveTravelled / (wavePeriod * scale * pow(1.1f, (float)waveNum - 1.0f));

            //float signedDistanceToWaveCentre = dot(waveDirRight, flatPosition);
            float waveBreakScaleMod = 1.0f;// sin(signedDistanceToWaveCentre * 0.05f + waveAngle * 1024.0f + gameTime * waveSpeed * 0.06f + initialWaveDist * 0.2f) * 0.15f + 0.85f;

            float xOffset = cos(waveAngle) * cos(angle) * waveScale * scale * waveBreakScaleMod * windScaleModifier;
            float yOffset = sin(angle) * waveScale * scale * waveBreakScaleMod * windScaleModifier;
            float zOffset = sin(waveAngle) * cos(angle) * waveScale * scale * waveBreakScaleMod * windScaleModifier;

            finalOffset += float3(xOffset, yOffset, zOffset);
        }

        scale *= waveScaleMultiplier;

        waveNum++;
    }

    return finalOffset;
}

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