
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

float3 getFourierOffset(float3 position)
{
    float3 flatPosition = float3(position.x, 0.0f, position.z);
    float3 finalOffset = float3(0.0f, 0.0f, 0.0f);

    float scale = 0.5f;

    int waveNum = 0;

    [unroll(20)]
    while (waveNum < waveCount)
    {
        float waveAngle = hash11((float)waveNum * waveSeed) * waveSeed;
        float3 waveDir = float3(cos(waveAngle), 0.0f, sin(waveAngle));

        float distWaveTravelled = gameTime * waveSpeed * scale + dot(flatPosition, waveDir);

        float angle = distWaveTravelled / (wavePeriod * scale * pow(1.2f, (float)waveNum - 1.0f));

        float xOffset = cos(waveAngle) * cos(angle) * waveScale * scale;
        float yOffset = sin(angle) * waveScale * scale;
        float zOffset = sin(waveAngle) * cos(angle) * waveScale * scale;

        scale *= waveScaleMultiplier;

        finalOffset += float3(xOffset, yOffset, zOffset);

        waveNum++;
    }

    return finalOffset;
}

static const float3 seaBaseColour = float3(0.0f, 0.09f, 0.18f);
static const float3 seaWaterColour = float3(0.0f, 0.45f, 0.65f) * 0.7f;
static const float sampleOffset = 0.025f;

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
    float3 viewDirection = normalize(cameraPosition - input.baseWorldPos);

    float3 tangentFlatWorldPos = float3(input.baseWorldPos.x + sampleOffset, input.baseWorldPos.y, input.baseWorldPos.z);
    float3 bitangentFlatWorldPos = float3(input.baseWorldPos.x, input.baseWorldPos.y, input.baseWorldPos.z + sampleOffset);

    float3 mainFourierOffset = getFourierOffset(input.baseWorldPos);
    float3 tangentFourierOffset = getFourierOffset(tangentFlatWorldPos);
    float3 bitangentFourierOffset = getFourierOffset(bitangentFlatWorldPos);

    float3 mainFourierPosition = input.baseWorldPos + mainFourierOffset;
    float3 tangentFourierPosition = tangentFlatWorldPos + tangentFourierOffset;
    float3 bitangentFourierPosition = bitangentFlatWorldPos + bitangentFourierOffset;

    float3 tangentDiff = tangentFourierPosition - mainFourierPosition;
    float3 bitangentDiff = bitangentFourierPosition - mainFourierPosition;

    float3 tangent = normalize(tangentDiff);
    float3 bitangent = normalize(bitangentDiff);
    float3 normal = normalize(cross(bitangent, tangent));

    float fresnel = clamp(1.0f - dot(normal, viewDirection), 0.0f, 1.0f);
    fresnel = pow(fresnel, 5.0f) * 0.25f;

    float3 reflected = float3(1.0f, 1.0f, 1.0f);//getSkyColor(reflect(viewDirection, normal));
    float3 refracted = seaBaseColour + diffuse(normal, -lightDirection, 20.0f) * seaWaterColour * 0.12f;

    float3 colour = lerp(refracted, reflected, fresnel);

    float cameraDist = distance(mainFourierPosition, cameraPosition);
    float waterAlpha = getLinearProgress(1500.0f, 650.0f, cameraDist);
    float foamDistMod = getLinearProgress(350.0f, 10.0f, cameraDist);
    float heightDistMod = getLinearProgress(250.0f, 800.0f, cameraDist);

    float lengthOfDiffs = (length(tangentDiff) * length(bitangentDiff)) / pow(sampleOffset, 2.0f);
    float foam = smoothstep(foamStart, 0.0f, lengthOfDiffs) * foamDistMod * 0.85f;
    colour += float3(1.0f, 1.0f, 1.0f) * foam;

    //float height = smoothstep(colourChangeStart, 0.0f, lengthOfDiffs);
    float height = clamp((mainFourierOffset.y - input.baseWorldPos.y + waveScale * 1.0f - colourChangeStart * waveScale * 2.0f) / (waveScale * 2.0f) + heightDistMod, 0.0f, 1.0f);

    colour += seaWaterColour * height;

    float specularFactor = specular(normal, lightDirection, viewDirection, 20.0f) * 0.5f;
    colour += float3(specularFactor, specularFactor, specularFactor);

    float3 factor = float3(waterAlpha, waterAlpha, waterAlpha);

    return float4(colour, waterAlpha);
}
