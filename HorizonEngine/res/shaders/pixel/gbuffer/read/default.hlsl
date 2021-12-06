
struct PS_INPUT
{
    float4 pos : SV_POSITION;
    float2 texCoord : TEXCOORD;
};

SamplerState samplerState : SAMPLER : register(s0);

Texture2D albedoTexture : TEXTURE: register(t0);
Texture2D positionRoughnessTexture : TEXTURE: register(t1);
Texture2D normalAOTexture : TEXTURE: register(t2);
Texture2D height : TEXTURE: register(t3);
Texture2D emissionMetallicTexture : TEXTURE: register(t4);

float4 main(PS_INPUT input) : SV_TARGET
{
    float3 viewDirection = normalize(cameraPosition - input.worldPos);

    float3 normal = normalize(input.normal);
    float3 tangent = normalize(input.tangent);
    float3 bitangent = normalize(input.bitangent);

    float3x3 tangentToWorld = float3x3(tangent, bitangent, normal);
    float3x3 worldToTangent = transpose(tangentToWorld);

    float2 unmodifiedTexCoord = input.texCoord;
    if (useParallaxOcclusionMapping)
    {
        float3 tangentViewDir = mul(viewDirection, worldToTangent);
        input.texCoord = getParallaxTextureCoords(tangentViewDir, input.texCoord);

        if (miscToggleA)
        {
            if (input.texCoord.x < 0.0f || input.texCoord.x > 1.0f || input.texCoord.y < 0.0f || input.texCoord.y > 1.0f)
            {
                discard;
            }
        }
    }

    if (showUVs)
    {
        return float4(frac(input.texCoord * 40.0f), 0.0f, 1.0f);
    }

    float3 textureColour = diffuseTexture.Sample(samplerState, input.texCoord).rgb;

    float specularPower = 16.0f;
    float specularMultiplier = 0.25f;

    if (roughnessMapping)
    {
        float roughnessSample = specularTexture.Sample(samplerState, input.texCoord).r;
        specularMultiplier = (1.0f - roughnessSample);
        specularPower = lerp(1.0f, 128.0f, pow(1.0f - roughnessSample, 3.0f));
    }

    if (useNormalMapping)
    {
        float3 normalColour = normalTexture.Sample(samplerState, input.texCoord).rgb;

        if (miscToggleB)
        {
            return float4(normalColour, 1.0f);
        }

        normalColour = normalize(normalColour * 2.0f - 1.0f);
        normal = normalize(mul(normalColour, tangentToWorld));

        if (cullBackNormals && dot(viewDirection, normal) < 0.0f) discard;
    }

    if (showWorldNormals)
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
        float3 ambient = directionalLight.colour * textureColour * directionalLight.ambientStrength;

        float3 toLight = -directionalLight.direction;

        float diffuseFloat = max(dot(toLight, normal), 0.0f);
        float3 diffuse = diffuseFloat * directionalLight.colour * textureColour;

        float3 reflectDirection = reflect(-toLight, normal);

        float specularFloat = pow(max(dot(viewDirection, reflectDirection), 0.0), specularPower);
        float3 specular = specularFloat * directionalLight.colour * objectMaterial.specularity * specularMultiplier;

        //return float4(specularFloat, specularFloat, specularFloat, 1.0f);

        float directionalLightShadowFactor = 1.0f;
        if (selfShadowing)
        {
            float3 toLightTangent = mul(toLight, worldToTangent);
            directionalLightShadowFactor = getShadowFactorForLightAtCoord(toLightTangent, input.texCoord, true);
        }

        cumulativeColour += ambient + (diffuse + specular) * directionalLightShadowFactor;
    }

    // Point light
    {
        for (int i = 0; i < numPointLights; ++i)
        {
            float pointLightDistance = distance(pointLights[i].position, input.worldPos);
            float pointLightAttenuation = 1.0f / (pointLights[i].attenuationConstant + pointLights[i].attenuationLinear * pointLightDistance + pointLights[i].attenuationQuadratic * pow(pointLightDistance, 2.0f));
            
            //float pointLightAttenuation = pow(1.0f - min(1.0f, pointLightDistance / 15.0f), 2.0f);
            
            if (pointLightAttenuation > 0.01f)
            {
                float3 toLight = normalize(pointLights[i].position - input.worldPos);

                float diffuseFloat = max(dot(toLight, normal), 0.0f);
                float3 diffuse = diffuseFloat * pointLights[i].colour * textureColour;

                float3 reflectDirection = reflect(-toLight, normal);

                float specularFloat = pow(max(dot(viewDirection, reflectDirection), 0.0), specularPower);
                float3 specular = specularFloat * pointLights[i].colour * specularMultiplier;

                float lightShadowFactor = 1.0f;
                if (selfShadowing)
                {
                    float3 toLightTangent = mul(toLight, worldToTangent);
                    lightShadowFactor = getShadowFactorForLightAtCoord(toLightTangent, input.texCoord, true);
                }

                cumulativeColour += (diffuse + specular) * pointLightAttenuation * lightShadowFactor;
            }
        }
    }
    // Spot light
    {
        for (int i = 0; i < numSpotLights; ++i)
        {
            float spotLightDistance = distance(spotLights[i].position, input.worldPos);
            float spotLightAttenuation = 1.0f / (spotLights[i].attenuationConstant + spotLights[i].attenuationLinear * spotLightDistance + spotLights[i].attenuationQuadratic * pow(spotLightDistance, 2.0f));
            
            //float spotLightAttenuation = pow(1.0f - min(1.0f, spotLightDistance / 15.0f), 2.0f);
            
            if (spotLightAttenuation > 0.01f)
            {
                float3 toLight = normalize(spotLights[i].position - input.worldPos);

                float lightAngle = (acos(dot(-toLight, spotLights[i].direction)) * 180.0) / 3.141592f;
                float lightCutoffAmount = 1.0 - smoothstep(spotLights[i].innerCutoff, spotLights[i].outerCutoff, lightAngle);
                if (lightCutoffAmount > 0.001f)
                {
                    float diffuseFloat = max(dot(toLight, normal), 0.0f);
                    float3 diffuse = diffuseFloat * spotLights[i].colour * textureColour;

                    float3 reflectDirection = reflect(-toLight, normal);

                    float specularFloat = pow(max(dot(viewDirection, reflectDirection), 0.0), specularPower);
                    float3 specular = specularFloat * spotLights[i].colour * specularMultiplier;

                    float lightShadowFactor = 1.0f;
                    if (selfShadowing)
                    {
                        float3 toLightTangent = mul(toLight, worldToTangent);
                        lightShadowFactor = getShadowFactorForLightAtCoord(toLightTangent, input.texCoord, true);
                    }

                    //cumulativeColour += (ambient + diffuse + specular) * spotLightAttenuation;
                    cumulativeColour += (diffuse + specular) * spotLightAttenuation * lightCutoffAmount * lightShadowFactor;
                }
            }
        }
    }

    float fresnelFactor = clamp(1.0f - dot(-normal, -viewDirection), 0.0f, 1.0f);
    fresnelFactor = pow(fresnelFactor, 5.0f) * 0.5f;

    cumulativeColour = lerp(cumulativeColour, directionalLight.colour, fresnelFactor);

    if (gammaCorrection)
    {
        cumulativeColour = pow(cumulativeColour, 1.0f / 2.2f);
    }

    return float4(cumulativeColour, 1.0f);
}