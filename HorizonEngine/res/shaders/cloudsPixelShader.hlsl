
cbuffer constantBuffer : register(b0)
{
    float3 cameraPosition;
    float padding1;

    float3 lightDirection;
    float  cloudCoverage;

    float3 lightColour;
    float  gameTime;

    float lightAbsorbtionThroughClouds;
    float lightAbsorbtionTowardsSun;
    float phaseFactor;
    float darknessThreshold;

    float cloudSpeed;
    float cloudHeight;
    int   numSteps;
    float stepSize;
};

struct PS_INPUT
{
    float4 pos : SV_POSITION;
    float3 normal : NORMAL;
    float2 texCoord : TEXCOORD;
    float3 worldPos : WORLD_POSIITION;
    float3x3 TBNMatrix : TBN_MATRIX;
};

//float GetDensityAt(float3 position)
//{
//    float3 modPos = float3(abs(fmod(position.x, 10.0f)), position.y, abs(fmod(position.z, 10.0f)));
//    float dist = distance(modPos, float3(5.0f, 3.0f, 5.0f));
//    return smoothstep(3.0f, 5.5f, dist);
//}

Texture3D<float> noiseTexture : TEXTURE3D : register(t0);
SamplerState samplerState : SAMPLER : register(s0);

static const float randomConstant = 50.0f;

float getLinearProgress(float edge0, float edge1, float x)
{
    return clamp((x - edge0) / (edge1 - edge0), 0.0f, 1.0f);
}

float hash11(float p)
{
    p = frac(p * 0.1031f);
    p *= p + 33.33f;
    p *= p + p;
    return frac(p);
}

float cloudNoise(float3 x)
{
    // The noise function returns a value in the range -1.0f -> 1.0f

    /*float3 p = floor(x);
    float3 f = frac(x);

    f = f * f * (3.0f - 2.0f * f);
    float n = p.x + p.y * 57.0f + 113.0f * p.z;

    return lerp(lerp(lerp(hash11(n), hash11(n + 1.0f), f.x),
        lerp(hash11(n + 57.0f), hash11(n + 58.0f), f.x), f.y),
        lerp(lerp(hash11(n + 113.0f), hash11(n + 114.0f), f.x),
            lerp(hash11(n + 170.0f), hash11(n + 171.0f), f.x), f.y), f.z);*/

    return noiseTexture.Sample(samplerState, x).r;
}

float getDensityAt(float3 position)
{
    float total = cloudNoise(float3(position.x * 0.00005f, position.y * 0.0002f, position.z * 0.0001f - gameTime * cloudSpeed)) * 0.65;
    total += cloudNoise(position * 0.00015f + 1240.0f) * 0.25f;
    total += cloudNoise(position * 0.001f + 142450.0f) * 0.1f;

    float halfCloudHeight = cloudHeight * 0.5f;
    float halfDiff = position.y - halfCloudHeight;
    if (halfDiff > 0.0f)
    {
        float normalisedDistToEdge = min(halfCloudHeight, halfDiff) / halfCloudHeight;
        total -= getLinearProgress(0.5f, 1.0f, normalisedDistToEdge);
    }
    else
    {
        float normalisedDistToEdge = min(halfCloudHeight, -halfDiff) / halfCloudHeight;
        total -= pow(getLinearProgress(0.5f, 1.0f, normalisedDistToEdge), 2.0f);
    }

    //total = lerp(min(1.0f, total), 0.0f, 1.0f - cloudCoverage);
   
    total -= 1.0f - cloudCoverage;
    //total = total * 2.0f - 1.0f;

    return total;
}

float getLight(float3 position, float noiseOffset, float step)
{
    int numLightSteps = 3;

    float totalDensity = 0.0f;
    step = 50.0f;

    position -= noiseOffset * lightDirection * step;

    [unroll(numLightSteps)]
    while (true)
    {
        position -= lightDirection * step;
        totalDensity += max(0.0f, getDensityAt(position));
    }

    float transmittance = exp(-totalDensity * lightAbsorbtionTowardsSun * randomConstant);
    return darknessThreshold + transmittance * (1.0f - darknessThreshold);
}

float4 main(PS_INPUT input) : SV_TARGET
{
    float3 cameraFragPosDiff = input.worldPos - cameraPosition;
    float flatCameraFragDist = length(float3(cameraFragPosDiff.x, 0.0f, cameraFragPosDiff.z));

    float3 mainRayDirection = normalize(cameraFragPosDiff);

    float angle = acos(dot(mainRayDirection, float3(0.0f, -1.0f, 0.0f))) - 1.570796f;

    //float distCloudHeightMod = getLinearProgress(7000.0f, 0.0f, flatCameraFragDist);

    float dist = (cloudHeight) / sin(angle); // * distCloudHeightMod
    float step = dist / (float)numSteps;

    float noiseOffset = cloudNoise(input.worldPos * 5.0f);
    float3 rayPos = float3(input.worldPos.x, 0.0f, input.worldPos.z) + mainRayDirection * noiseOffset * step;

    float rayDist = 0.0f;
    int stepsMade = 0;

    float transmittance = 1.0f;
    float lightEnergy = 0.0f;

    [unroll(100)]
    while(rayDist < dist && stepsMade < numSteps)
    {
        float density = getDensityAt(rayPos);

        if (density > 0.0f)
        {
            float lightTransmittance = getLight(rayPos, noiseOffset, step);
            lightEnergy += density * transmittance * lightTransmittance * phaseFactor * randomConstant;

            transmittance *= exp(-density * lightAbsorbtionThroughClouds * randomConstant);

            if (transmittance < 0.01f)
            {
                break;
            }
        }

        rayPos += mainRayDirection * step;
        rayDist += step;
        stepsMade += 1;
    }

    //float transmittance = exp(-density * oneOverSampleCount * 2.0f);
    
    float sunDot = dot(normalize(input.worldPos - cameraPosition), -lightDirection);
    float lightIntensity = clamp((sunDot - 0.9f) / (1.0f - 0.9f), 0.0, 1.0);
    lightIntensity = pow(lightIntensity, 10.0f) * 1.5f;

    lightEnergy += lightEnergy * lightIntensity;

    float alpha = 1.0f - getLinearProgress(3500.0f, 20000.0f, flatCameraFragDist);
    float3 cloudColour = lightColour * lightEnergy;
    return float4(cloudColour, alpha * (1.0f - transmittance));
}