
#include "../../shared/standard.hlsli"
#include "../../shared/scene.hlsli"
#include "../../shared/water.hlsli"


/***********************************************

MARKING SCHEME: Recent / Advanced graphics algorithms or techniques

DESCRIPTION: This is the shader used to shade the water in the
default scene

COMMENT INDEX: 20

***********************************************/

float hash11(float p)
{
    p = frac(p * 0.1031f);
    p *= p + 33.33f;
    p *= p + p;
    return frac(p);
}

Texture3D<float> noiseTexture : TEXTURE3D: register(t0);
SamplerState samplerState : SAMPLER: register(s0);

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

float4 main(VSPS_TRANSFER_WATER input) : SV_TARGET
{
	float3 modifiedLightColour = float3(max(cb_directionalLight.colour.r, 0.05f), max(cb_directionalLight.colour.g, 0.1f), max(cb_directionalLight.colour.b, 0.15f));

    float3 viewDirection = normalize(input.baseWorldPos - cb_cameraPosition);

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

    float cameraDist = distance(mainFourierPosition, cb_cameraPosition);
    float waterAlpha = getLinearProgress(1500.0f, 650.0f, cameraDist);
    float foamDistMod = getLinearProgress(450.0f, 10.0f, cameraDist);
    float heightDistMod = -getLinearProgress(450.0f, 1500.0f, cameraDist);

    float fresnel = clamp(1.0f - dot(-normal, viewDirection), 0.0f, 1.0f);
    fresnel = pow(fresnel, 10.0f) * 0.35f;

	float3 refracted = seaBaseColour * modifiedLightColour + diffuse(normal, -cb_directionalLight.direction, 20.0f) * seaWaterColour * 0.12f;
    float3 reflected = modifiedLightColour;//getSkyColor();// max(0.0f, sign(reflect(viewDirection, normal).y))
    float3 colour = refracted;// lerp(refracted, reflected, fresnel);

   // float height = getLinearProgress(-1.0f, 1.0f, (mainFourierOffset.y - input.baseWorldPos.y) / waveScale);
   // height = clamp(height + colourChangeStart + heightDistMod, 0.0f, 1.0f);
    float height = clamp((mainFourierOffset.y - input.baseWorldPos.y + cb_waveScale * 1.0f - cb_colourChangeStart * cb_waveScale * 2.0f) / (cb_waveScale * 2.0f) + heightDistMod, 0.0f, 1.0f);
    colour += seaWaterColour * height * modifiedLightColour;

    float lengthOfDiffs = (length(tangentDiff) * length(bitangentDiff)) / pow(sampleOffset, 2.0f);
    float foam = getLinearProgress(cb_foamStart, 0.0f, lengthOfDiffs) * foamDistMod;
    colour += foam * modifiedLightColour;

    float foamSpecular = lerp(1.0f, 80.0f, pow(1.0f - foam, 5.0f));
	float specularFactor = specular(normal, -cb_directionalLight.direction, viewDirection, foamSpecular) * 0.25f;
    colour += float3(specularFactor, specularFactor, specularFactor) * modifiedLightColour;

    //colour = float3(fresnel, fresnel, fresnel);

    return float4(colour, waterAlpha);
}
