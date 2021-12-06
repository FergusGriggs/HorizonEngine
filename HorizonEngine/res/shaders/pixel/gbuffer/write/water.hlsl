
struct PS_INPUT
{
    float4 pos : SV_POSITION;
    float3 baseWorldPos : BASE_WORLD_POSIITION;
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
    fresnel = pow(fresnel, 10.0f) * 0.35f;

    float3 refracted = seaBaseColour * modifiedLightColour + diffuse(normal, -lightDirection, 20.0f) * seaWaterColour * 0.12f;
    float3 reflected = modifiedLightColour;//getSkyColor();// max(0.0f, sign(reflect(viewDirection, normal).y))
    float3 colour = refracted;// lerp(refracted, reflected, fresnel);

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

struct PS_INPUT
{
    float4 pos : SV_POSITION;
    float3 normal : NORMAL;
    float2 texCoord : TEXCOORD;
    float3 worldPos : WORLD_POSITION;

    float3 tangent : TANGENT;
    float3 bitangent : BITANGENT;
};

struct PS_OUTPUT
{
    float4 albedo : SV_TARGET0;
    float4 positionRoughness : SV_TARGET1;
    float4 normalAO : SV_TARGET2;
    float  depth : SV_TARGET3;
    float4 emissionMetallic : SV_TARGET4;
};

Texture2D albedoTexture : TEXTURE: register(t0);
Texture2D roughnessTexture : TEXTURE: register(t1);
Texture2D normalTexture : TEXTURE: register(t2);
Texture2D ambientOcclusionTexture : TEXTURE: register(t3);
Texture2D metallicTexture : TEXTURE: register(t4);
Texture2D emissionTexture : TEXTURE: register(t5);
Texture2D depthTexture : TEXTURE: register(t6);

SamplerState objSamplerState : SAMPLER: register(s0);

PS_OUTPUT main(PS_INPUT input)
{
    PS_OUTPUT output;

    // Output albedo
    float4 albedo = albedoTexture.Sample(objSamplerState, input.texCoord);
    output.albedo = float4(pow(albedo.rgb, 2.2f), albedo.a);

    // Output position
    output.positionRoughness.rgb = input.worldPos;

    // Output roughness
    output.positionRoughness.a = roughnessTexture.Sample(objSamplerState, input.texCoord).r;

    // Output normal
    float3 texturedNormal = normalTexture.Sample(objSamplerState, input.texCoord).xyz;
    output.normalAO.rgb = normalize(mul(normalize(texturedNormal * 2.0f - 1.0f), input.TBNMatrix));

    // Output ambient occlusion
    output.normalAO.a = ambientOcclusionTexture.Sample(objSamplerState, input.texCoord).r;

    // Output depth
    output.depth.r = depthTexture.Sample(objectSamplerState, input.texCoord).r;

    // Output emission
    output.emissionMetallic.rgb = emissionTexture.Sample(objSamplerState, input.texCoord).rgb;

    // Output metallic
    output.emissionMetallic.a = metallicTexture.Sample(objSamplerState, input.texCoord).r;

    return output;
}