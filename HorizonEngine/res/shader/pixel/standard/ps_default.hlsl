
#include "../../shared/standard.hlsli"
#include "../../shared/scene.hlsli"
#include "../../shared/POM.hlsli"

struct PS_INPUT
{
	float4 pos : SV_POSITION;
	float3 normal : NORMAL;
	float2 texCoord : TEXCOORD;
	float3 worldPos : WORLD_POSIITION;

	float3 tangent : TANGENT;
	float3 bitangent : BITANGENT;
};

SamplerState samplerState : SAMPLER : register(s0);

Texture2D albedoTexture : TEXTURE : register(t0);
Texture2D roughnessTexture : TEXTURE : register(t1);
Texture2D normalTexture : TEXTURE : register(t2);
Texture2D ambientOcclusionTexture : TEXTURE : register(t3);
Texture2D metallicTexture : TEXTURE : register(t4);
Texture2D emissionTexture : TEXTURE : register(t5);
Texture2D depthTexture : TEXTURE : register(t6);

float4 main(PS_INPUT input) : SV_TARGET
{
    float3 viewDirection = normalize(cb_cameraPosition - input.worldPos);

    float3 normal = normalize(input.normal);
    float3 tangent = normalize(input.tangent);
    float3 bitangent = normalize(input.bitangent);

    float3x3 tangentToWorld = float3x3(tangent, bitangent, normal);
    float3x3 worldToTangent = transpose(tangentToWorld);

    float2 unmodifiedTexCoord = input.texCoord;
    if (cb_useParallaxOcclusionMapping)
    {
        float3 tangentViewDir = mul(viewDirection, worldToTangent);
        input.texCoord = getParallaxTextureCoords(tangentViewDir, input.texCoord, depthTexture, cb_depthScale);

        if (cb_miscToggleA)
        {
            if (input.texCoord.x < 0.0f || input.texCoord.x > 1.0f || input.texCoord.y < 0.0f || input.texCoord.y > 1.0f)
            {
                discard;
            }
        }
    }

	if (cb_showUVs)
    {
        return float4(frac(input.texCoord * 40.0f), 0.0f, 1.0f);
    }

	float3 albedo = albedoTexture.Sample(samplerState, input.texCoord).rgb;

    float specularPower = 16.0f;
    float specularMultiplier = 0.25f;

    if (cb_roughnessMapping)
    {
        float roughnessSample = roughnessTexture.Sample(samplerState, input.texCoord).r;
        specularMultiplier = (1.0f - roughnessSample);
        specularPower = lerp(1.0f, 128.0f, pow(1.0f - roughnessSample, 3.0f));
    }

	if (cb_useNormalMapping)
    {
		float3 normalColour = normalTexture.Sample(samplerState, input.texCoord).rgb;

		if (cb_miscToggleB)
        {
            return float4(normalColour, 1.0f);
        }

        normalColour = normalize(normalColour * 2.0f - 1.0f);
        normal = normalize(mul(normalColour, tangentToWorld));

        if (cb_cullBackNormals && dot(viewDirection, normal) < 0.0f) discard;
    }

	if (cb_showWorldNormals)
    {
        return float4(normal * 0.5f + 0.5f, 1.0f);
    }

    float tangentProgress = frac(dot(input.worldPos, tangent));
    float bitangentProgress = frac(dot(input.worldPos, bitangent));
    //float normalProgress = frac(dot(input.worldPos, normal));
    // return float4(tangentProgress, bitangentProgress, 0.0f, 1.0f);
    //return float4(normalProgress, normalProgress, normalProgress, 1.0f);

    float3 cumulativeColour = float3(0.0f, 0.0f, 0.0f);

    // Directional light
    {
		float3 ambient = cb_directionalLight.colour * albedo * cb_directionalLight.ambientStrength;

		float3 toLight = -cb_directionalLight.direction;

        float diffuseFloat = max(dot(toLight, normal), 0.0f);
		float3 diffuse = diffuseFloat * cb_directionalLight.colour * albedo;

        float3 reflectDirection = reflect(-toLight, normal);

        float specularFloat = pow(max(dot(viewDirection, reflectDirection), 0.0), specularPower);
		float3 specular = specularFloat * cb_directionalLight.colour * specularMultiplier;

        //return float4(specularFloat, specularFloat, specularFloat, 1.0f);

        float directionalLightShadowFactor = 1.0f;
		if (cb_selfShadowing)
        {
            float3 toLightTangent = mul(toLight, worldToTangent);
			directionalLightShadowFactor = getShadowFactorForLightAtCoord(toLightTangent, input.texCoord, true, depthTexture, cb_depthScale);
		}

        cumulativeColour += ambient + (diffuse + specular) * directionalLightShadowFactor;
    }

    // Point light
    {
		for (int i = 0; i < cb_numPointLights; ++i)
        {
			float pointLightDistance = distance(cb_pointLights[i].position, input.worldPos);
			float pointLightAttenuation = 1.0f / (cb_pointLights[i].attenuationConstant + cb_pointLights[i].attenuationLinear * pointLightDistance + cb_pointLights[i].attenuationQuadratic * pow(pointLightDistance, 2.0f));
            
            //float pointLightAttenuation = pow(1.0f - min(1.0f, pointLightDistance / 15.0f), 2.0f);
            
            if (pointLightAttenuation > 0.01f)
            {
				float3 toLight = normalize(cb_pointLights[i].position - input.worldPos);

                float diffuseFloat = max(dot(toLight, normal), 0.0f);
				float3 diffuse = diffuseFloat * cb_pointLights[i].colour * albedo;

                float3 reflectDirection = reflect(-toLight, normal);

                float specularFloat = pow(max(dot(viewDirection, reflectDirection), 0.0), specularPower);
				float3 specular = specularFloat * cb_pointLights[i].colour * specularMultiplier;

                float lightShadowFactor = 1.0f;
				if (cb_selfShadowing)
                {
                    float3 toLightTangent = mul(toLight, worldToTangent);
					lightShadowFactor = getShadowFactorForLightAtCoord(toLightTangent, input.texCoord, true, depthTexture, cb_depthScale);
				}

                cumulativeColour += (diffuse + specular) * pointLightAttenuation * lightShadowFactor;
            }
        }
    }
    // Spot light
    {
		for (int i = 0; i < cb_numSpotLights; ++i)
        {
			float spotLightDistance = distance(cb_spotLights[i].position, input.worldPos);
			float spotLightAttenuation = 1.0f / (cb_spotLights[i].attenuationConstant + cb_spotLights[i].attenuationLinear * spotLightDistance + cb_spotLights[i].attenuationQuadratic * pow(spotLightDistance, 2.0f));
            
            //float spotLightAttenuation = pow(1.0f - min(1.0f, spotLightDistance / 15.0f), 2.0f);
            
            if (spotLightAttenuation > 0.01f)
            {
				float3 toLight = normalize(cb_spotLights[i].position - input.worldPos);

				float lightAngle = (acos(dot(-toLight, cb_spotLights[i].direction)) * 180.0) / 3.141592f;
				float lightCutoffAmount = 1.0 - smoothstep(cb_spotLights[i].innerCutoff, cb_spotLights[i].outerCutoff, lightAngle);
                if (lightCutoffAmount > 0.001f)
                {
                    float diffuseFloat = max(dot(toLight, normal), 0.0f);
					float3 diffuse = diffuseFloat * cb_spotLights[i].colour * albedo;

                    float3 reflectDirection = reflect(-toLight, normal);

                    float specularFloat = pow(max(dot(viewDirection, reflectDirection), 0.0), specularPower);
					float3 specular = specularFloat * cb_spotLights[i].colour * specularMultiplier;

                    float lightShadowFactor = 1.0f;
					if (cb_selfShadowing)
                    {
                        float3 toLightTangent = mul(toLight, worldToTangent);
						lightShadowFactor = getShadowFactorForLightAtCoord(toLightTangent, input.texCoord, true, depthTexture, cb_depthScale);
					}

                    //cumulativeColour += (ambient + diffuse + specular) * spotLightAttenuation;
                    cumulativeColour += (diffuse + specular) * spotLightAttenuation * lightCutoffAmount * lightShadowFactor;
                }
            }
        }
    }

    float fresnelFactor = clamp(1.0f - dot(-normal, -viewDirection), 0.0f, 1.0f);
    fresnelFactor = pow(fresnelFactor, 5.0f) * 0.5f;

	cumulativeColour = lerp(cumulativeColour, cb_directionalLight.colour, fresnelFactor);

	if (cb_gammaCorrection)
    {
        cumulativeColour = pow(cumulativeColour, 1.0f / 2.2f);
    }

    return float4(cumulativeColour, 1.0f);
}