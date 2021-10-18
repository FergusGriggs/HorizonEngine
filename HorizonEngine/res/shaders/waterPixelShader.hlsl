
// Water pixel shader

cbuffer constantBuffer : register(b0)
{
    //PACK_SEAM
    float3 cameraPosition;
    float gameTime;

    //Water parameters
    int waveCount;
    float waveScale;
    float wavePeriod;
    float waveSpeed;

    float waveSeed;
    float waveScaleMultiplier;
    float foamStart;
    float colourChangeStart;

    float3 lightDirection;
    int    iscolateWaveNum;

    float3 lightColour;
    float padding2;
};

struct PS_INPUT
{
    float4 pos : SV_POSITION;
    float3 baseWorldPos : WORLD_POSIITION;
};

float hash11(float p)
{
    p = frac(p * 0.1031f);
    p *= p + 33.33f;
    p *= p + p;
    return frac(p);
}

static float windDir = float3(1.0f, 0.0f, 0.0f);

Texture3D<float> noiseTexture : TEXTURE3D: register(t0);
SamplerState samplerState : SAMPLER: register(s0);

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
            float waveAngle = hash11((float)waveNum * waveSeed) * waveSeed;
            float3 waveDir = float3(cos(waveAngle), 0.0f, sin(waveAngle));
            //float3 waveDirRight = float3(waveDir.z, 0.0f, -waveDir.x);

            float windScaleModifier = dot(waveDir, windDir) * 0.1f + 0.8f;

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

    finalOffset += float3(0.0f, noiseTexture.Sample(samplerState, (position + float3(0.0f, 0.5f, 0.0f)) * 0.04f) * waveScale * 0.05f, 0.0f);
    //finalOffset += float3(0.0f, noiseTexture.Sample(samplerState, (position + finalOffset) * 0.1f) * 0.25f, 0.0f);

    //return float3(noiseTexture.Sample(samplerState, (position + finalOffset) * 0.1f), 0.0f, 0.0f);

    return finalOffset;
}

static const float3 seaBaseColour = float3(0.0f, 0.09f, 0.18f);
static const float3 seaWaterColour = float3(0.0f, 0.45f, 0.65f) * 0.7f;
static const float sampleOffset = 0.25f;

static const float pi = 3.141592f;

float diffuse(float3 normal, float3 light, float power)
{
    return pow(dot(normal, light) * 0.4f + 0.6f, power);
}

float specular(float3 normal, float3 light, float3 viewDirection, float s)
{
    float nrm = (s + 8.0f) / (pi * 8.0f);
    return pow(max(dot(reflect(viewDirection, normal), light), 0.0f), s) * nrm;
}

float getLinearProgress(float edge0, float edge1, float x)
{
    return clamp((x - edge0) / (edge1 - edge0), 0.0f, 1.0f);
}

float4 main(PS_INPUT input) : SV_TARGET
{
    float3 modifiedLightColour = float3(max(lightColour.r, 0.05f), max(lightColour.g, 0.1f), max(lightColour.b, 0.15f));

    float3 viewDirection = normalize(input.baseWorldPos - cameraPosition);

    float3 tangentFlatWorldPos = float3(input.baseWorldPos.x + sampleOffset, input.baseWorldPos.y, input.baseWorldPos.z);
    float3 bitangentFlatWorldPos = float3(input.baseWorldPos.x, input.baseWorldPos.y, input.baseWorldPos.z + sampleOffset);

    float3 mainFourierOffset = getFourierOffset(input.baseWorldPos);
    float3 tangentFourierOffset = getFourierOffset(tangentFlatWorldPos);
    float3 bitangentFourierOffset = getFourierOffset(bitangentFlatWorldPos);

    //return float4(mainFourierOffset, 1.0f);

    float3 mainFourierPosition = input.baseWorldPos + mainFourierOffset;
    float3 tangentFourierPosition = tangentFlatWorldPos + tangentFourierOffset;
    float3 bitangentFourierPosition = bitangentFlatWorldPos + bitangentFourierOffset;

    float3 tangentDiff = tangentFourierPosition - mainFourierPosition;
    float3 bitangentDiff = bitangentFourierPosition - mainFourierPosition;

    float3 tangent = normalize(tangentDiff);
    float3 bitangent = normalize(bitangentDiff);
    float3 normal = normalize(cross(bitangent, tangent));

    float cameraDist = distance(mainFourierPosition, cameraPosition);
    float waterAlpha = getLinearProgress(1500.0f, 650.0f, cameraDist);
    float foamDistMod = getLinearProgress(450.0f, 10.0f, cameraDist);
    float heightDistMod = -getLinearProgress(450.0f, 1500.0f, cameraDist);

    float fresnel = clamp(1.0f - dot(-normal, viewDirection), 0.0f, 1.0f);
    fresnel = pow(fresnel, 10.0f) * 0.5f;

    float3 reflected = modifiedLightColour;//getSkyColor();// max(0.0f, sign(reflect(viewDirection, normal).y))
    float3 refracted = seaBaseColour * modifiedLightColour + diffuse(normal, -lightDirection, 20.0f) * seaWaterColour * 0.12f;
    float3 colour = lerp(refracted, reflected, fresnel);

   // float height = getLinearProgress(-1.0f, 1.0f, (mainFourierOffset.y - input.baseWorldPos.y) / waveScale);
   // height = clamp(height + colourChangeStart + heightDistMod, 0.0f, 1.0f);
    float height = clamp((mainFourierOffset.y - input.baseWorldPos.y + waveScale * 1.0f - colourChangeStart * waveScale * 2.0f) / (waveScale * 2.0f) + heightDistMod, 0.0f, 1.0f);
    colour += seaWaterColour * height * modifiedLightColour;

    float lengthOfDiffs = (length(tangentDiff) * length(bitangentDiff)) / pow(sampleOffset, 2.0f);
    float foam = getLinearProgress(foamStart, 0.0f, lengthOfDiffs) * foamDistMod;
    colour += foam * modifiedLightColour;

    float foamSpecular = lerp(1.0f, 80.0f, pow(1.0f - foam, 5.0f));
    float specularFactor = specular(normal, -lightDirection, viewDirection, foamSpecular) * 0.25f;
    colour += float3(specularFactor, specularFactor, specularFactor) * modifiedLightColour;

    //colour = float3(fresnel, fresnel, fresnel);

    return float4(colour, waterAlpha);
}
