
#include "../../shared/standard.hlsli"
#include "../../shared/scene.hlsli"
#include "../../shared/POM.hlsli"

float4 main(VSPS_TRANSFER input) : SV_TARGET
{
    float3 viewDirection = normalize(cb_cameraPosition - input.worldPos);
    
    float3 normal = normalize(input.normal);

    float3 albedo = float3(0.1f, 0.9f, 0.1f);

    float specularPower = 4.0f;
    float specularMultiplier = 0.25f;

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

        cumulativeColour += ambient + (diffuse + specular);
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

                cumulativeColour += (diffuse + specular) * pointLightAttenuation;
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

                    //cumulativeColour += (ambient + diffuse + specular) * spotLightAttenuation;
                    cumulativeColour += (diffuse + specular) * spotLightAttenuation * lightCutoffAmount;
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