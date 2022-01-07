
#include "../../shared/standard.hlsli"
#include "../../shared/scene.hlsli"
#include "../../shared/clouds.hlsli"

Texture3D<float> noiseTexture : TEXTURE3D : register(t0);
SamplerState samplerState : SAMPLER : register(s0);

static const float randomConstant = 50.0f;

/***********************************************

MARKING SCHEME: Recent / Advanced graphics algorithms or techniques

DESCRIPTION: This is the shader used to actually render the clouds,
it uses the 3d noise texture created using the compute shader. 

COMMENT INDEX: 18

***********************************************/

float getLinearProgress(float edge0, float edge1, float x)
{
    return clamp((x - edge0) / (edge1 - edge0), 0.0f, 1.0f);
}

//float hash11(float p)
//{
//    p = frac(p * 0.1031f);
//    p *= p + 33.33f;
//    p *= p + p;
//    return frac(p);
//}

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
	float total = cloudNoise(float3(position.x * 0.0001f, position.y * 0.0002f, position.z * 0.00015f - cb_gameTime * cb_cloudSpeed)) * 0.45;
	total += cloudNoise(float3(position.x * 0.00003f - cb_gameTime * cb_cloudSpeed * 0.4, position.y * 0.00005f, position.z * 0.00003f + cb_gameTime * cb_cloudSpeed * 0.5) + 1240.0f) * 0.45f;
	total += cloudNoise(float3(position.x * 0.001f, position.y * 0.001f, position.z * 0.001f - cb_gameTime * cb_cloudSpeed * 0.8) + 414.0f) * 0.1f;

	//float halfCloudHeight = cb_cloudHeight * 0.5f;
 //   float halfDiff = position.y - halfCloudHeight;
 //   if (halfDiff > 0.0f)
 //   {
 //       float normalisedDistToEdge = min(halfCloudHeight, halfDiff) / halfCloudHeight;
 //       total -= getLinearProgress(0.85f, 1.0f, normalisedDistToEdge);
 //   }
 //   else
 //   {
 //       float normalisedDistToEdge = min(halfCloudHeight, -halfDiff) / halfCloudHeight;
 //       total -= pow(getLinearProgress(0.85f, 1.0f, normalisedDistToEdge), 2.0f);
 //   }

    //total = lerp(min(1.0f, total), 0.0f, 1.0f - cloudCoverage);
   
	total -= 1.0f - cb_cloudCoverage;
    //total = total * 2.0f - 1.0f;

    return total;
}

float getLight(float3 position, float noiseOffset, float step)
{
    int numLightSteps = 3;

    float totalDensity = 0.0f;
    step = 50.0f;

	position -= noiseOffset * cb_directionalLight.direction * step;

    [unroll(numLightSteps)]
    while (true)
    {
		position -= cb_directionalLight.direction * step;
        totalDensity += max(0.0f, getDensityAt(position));
    }

    float transmittance = exp(-totalDensity * cb_lightAbsorbtionTowardsSun * randomConstant);
    return cb_darknessThreshold + transmittance * (1.0f - cb_darknessThreshold);
}

float4 main(VSPS_TRANSFER input) : SV_TARGET
{
    float3 cameraFragPosDiff = input.worldPos - cb_cameraPosition;
    float flatCameraFragDist = length(float3(cameraFragPosDiff.x, 0.0f, cameraFragPosDiff.z));

    float3 mainRayDirection = normalize(cameraFragPosDiff);

    float angle = acos(dot(mainRayDirection, float3(0.0f, -1.0f, 0.0f))) - 1.570796f;

    //float distCloudHeightMod = getLinearProgress(7000.0f, 0.0f, flatCameraFragDist);

    float dist = cb_cloudHeight / sin(angle); // * distCloudHeightMod
    float step = dist / (float)cb_numSteps;

    float noiseOffset = cloudNoise(input.worldPos * 5.0f);
    float3 rayPos = float3(input.worldPos.x, 0.0f, input.worldPos.z) + mainRayDirection * noiseOffset * step;

    float rayDist = 0.0f;
    int stepsMade = 0;

    float transmittance = 1.0f;
    float lightEnergy = 0.0f;

    [unroll(100)]
    while(rayDist < dist && stepsMade < cb_numSteps)
    {
        float density = getDensityAt(rayPos);

        if (density > 0.0f)
        {
            float lightTransmittance = getLight(rayPos, noiseOffset, step);
            lightEnergy += density * transmittance * lightTransmittance * cb_phaseFactor * randomConstant;

            transmittance *= exp(-density * cb_lightAbsorbtionThroughClouds * randomConstant);

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
    
	float sunDot = dot(normalize(input.worldPos - cb_cameraPosition), -cb_directionalLight.direction);
    float lightIntensity = clamp((sunDot - 0.9f) / (1.0f - 0.9f), 0.0, 1.0);
    lightIntensity = pow(lightIntensity, 10.0f) * 1.5f;

    lightEnergy += lightEnergy * lightIntensity;

    float alpha = 1.0f - getLinearProgress(3500.0f, 20000.0f, flatCameraFragDist);
	float3 cloudColour = cb_directionalLight.colour * lightEnergy;
    return float4(cloudColour, alpha * (1.0f - transmittance));
}