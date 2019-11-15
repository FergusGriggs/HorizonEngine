//pixelShader.hlsl
//Default pixel shader

cbuffer constantBuffer : register(b0)
{
    //PACK_SEAM // DIRECTIONAL LIGHT
    float3 directionalLightColour;
    float directionalLightAmbientStrength;

	//PACK_SEAM
    float directionalLightSpecularStrength;
    float3 directionalLightDirection;

	//PACK_SEAM
    float directionalLightShininess;
    float3 padding;

	//PACK_SEAM // POINT LIGHT
    float3 pointLightColour;
    float pointLightAmbientStrength;

	//PACK_SEAM
    float pointLightAttenuationConstant;
    float pointLightAttenuationLinear;
    float pointLightAttenuationQuadratic;
    float pointLightSpecularStrength;

	//PACK_SEAM
    float3 pointLightPosition;
    float pointLightShininess;

	//PACK_SEAM // SPOT LIGHT
    float3 spotLightColour;
    float spotLightAmbientStrength;

	//PACK_SEAM
    float spotLightAttenuationConstant;
    float spotLightAttenuationLinear;
    float spotLightAttenuationQuadratic;
    float spotLightSpecularStrength;

	//PACK_SEAM
    float3 spotLightPosition;
    float spotLightShininess;

	//PACK_SEAM
    float3 spotLightDirection;
    float spotLightInnerCutoff;

	//PACK_SEAM
    float spotLightOuterCutoff;
    float3 cameraPosition;

    //PACK_SEAM
    int useNormalMapping;
    int useParallaxOcclusionMapping;
    float parallaxOcclusionMappingHeight;
};

struct PS_INPUT
{
    float4 pos : SV_POSITION;
    float3 normal : NORMAL;
    float2 texCoord : TEXCOORD;
    float3 worldPos : WORLD_POSIITION;
    float3x3 TBNMatrix : TBN_MATRIX;
};

Texture2D diffuseTexture : TEXTURE : register(t0);
Texture2D specularTexture : TEXTURE : register(t1);
Texture2D normalTexture : TEXTURE : register(t2);
Texture2D depthTexture : TEXTURE : register(t3);

SamplerState samplerState : SAMPLER : register(s0);

float2 ParallaxMapping(float3 viewDir, float2 texCoords)
{
    const float heightScale = parallaxOcclusionMappingHeight;
    // number of depth layers
    const float minLayers = 32;
    const float maxLayers = 128;
    float numLayers = 128.0f; // minLayers + abs(dot(float3(0.0f, 0.0f, 1.0f), viewDir)) * (maxLayers - minLayers);

    // calculate the size of each layer
    float layerDepth = 1.0 / numLayers;
    // depth of current layer
    float currentLayerDepth = 0.0;
    // the amount to shift the texture coordinates per layer (from vector P)

    float2 P = viewDir.xy / viewDir.z * heightScale;
    float2 deltaTexCoords = P / numLayers;
  
    // get initial values
    float2 currentTexCoords = texCoords;
    float currentDepthMapValue = depthTexture.Sample(samplerState, currentTexCoords).r;
      
    for (int i = 0; i < numLayers; i++)
    {
        if (currentLayerDepth < currentDepthMapValue)
        {
            // shift texture coordinates along direction of P
            currentTexCoords -= deltaTexCoords;
        // get depthmap value at current texture coordinates
            currentDepthMapValue = depthTexture.Sample(samplerState, currentTexCoords).r;
        // get depth of next layer
            currentLayerDepth += layerDepth;
        }
        else
        {
            break;
        }
    }
    
    // get texture coordinates before collision (reverse operations)
    float2 prevTexCoords = currentTexCoords + deltaTexCoords;

    // get depth after and before collision for linear interpolation
    float afterDepth = currentDepthMapValue - currentLayerDepth;
    float beforeDepth = depthTexture.Sample(samplerState, prevTexCoords).r - currentLayerDepth + layerDepth;
 
    // interpolation of texture coordinates
    float weight = afterDepth / (afterDepth - beforeDepth);
    float2 finalTexCoords = prevTexCoords * weight + currentTexCoords * (1.0 - weight);

    return finalTexCoords;
}

float4 main(PS_INPUT input) : SV_TARGET
{
    float3 viewDirection = normalize(cameraPosition - input.worldPos);

    if (useParallaxOcclusionMapping)
    {
        float3 tangentCamPos = mul(cameraPosition, input.TBNMatrix);
        float3 tangentFragPos = mul(input.worldPos, input.TBNMatrix);
        float3 tangViewDir = normalize(tangentCamPos - tangentFragPos);
        input.texCoord = ParallaxMapping(tangViewDir, input.texCoord);
    }

    float3 textureColour = diffuseTexture.Sample(samplerState, input.texCoord);
    float3 specularColour = specularTexture.Sample(samplerState, input.texCoord);
    
    float3 normal;

    if (useNormalMapping) {
        float3 normalColour = normalTexture.Sample(samplerState, input.texCoord);
        normal = normalize(normalColour * 2.0f - 1.0f);
        //normal = normalize(mul(float3(normal.x, -normal.y, normal.z), input.TBNMatrix)); //float3(0.0f, 0.5f, 0.5f)
        normal = normalize(mul(normal, input.TBNMatrix)); //float3(0.0f, 0.5f, 0.5f)
    }
    else {
        normal = input.normal;

    }

    //normalColour = normal * 0.5f + 0.5f;
    //return float4(normalColour, 1.0f);

    float3 cumulativeColour = float3(0.0f, 0.0f, 0.0f);

    // DIRECTIONAL LIGHT
    {
        float3 ambient = directionalLightColour * directionalLightAmbientStrength * textureColour;

        float3 toLight = -directionalLightDirection;

        float diffuseFloat = max(dot(toLight, normal), 0.0f);
        float3 diffuse = diffuseFloat * directionalLightColour * textureColour;

        float3 reflectDirection = reflect(-toLight, normal);

        float specularFloat = pow(max(dot(viewDirection, reflectDirection), 0.0), directionalLightShininess);
        float3 specular = specularFloat * directionalLightColour * directionalLightSpecularStrength * specularColour;

        cumulativeColour += ambient + diffuse + specular;
    }
    // POINT LIGHT
    {
        float3 ambient = pointLightColour * pointLightAmbientStrength * textureColour;

        float3 toLight = normalize(pointLightPosition - input.worldPos);

        float diffuseFloat = max(dot(toLight, normal), 0.0f);
        float3 diffuse = diffuseFloat * pointLightColour * textureColour;

        float3 reflectDirection = reflect(-toLight, normal);

        float specularFloat = pow(max(dot(viewDirection, reflectDirection), 0.0), pointLightShininess);
        float3 specular = specularFloat * pointLightColour * pointLightSpecularStrength * specularColour;

        float pointLightDistance = distance(pointLightPosition, input.worldPos);
        float pointLightAttenuation = 1.0f / (pointLightAttenuationConstant + pointLightAttenuationLinear * pointLightDistance + pointLightAttenuationQuadratic * pow(pointLightDistance, 2.0f));

        cumulativeColour += (ambient + diffuse + specular) * pointLightAttenuation;
    }
    // SPOT LIGHT
    {
        float3 ambient = spotLightColour * spotLightAmbientStrength * textureColour;

        float3 toLight = normalize(spotLightPosition - input.worldPos);

        float diffuseFloat = max(dot(toLight, normal), 0.0f);
        float3 diffuse = diffuseFloat * spotLightColour * textureColour;

        float3 reflectDirection = reflect(-toLight, normal);

        float specularFloat = pow(max(dot(viewDirection, reflectDirection), 0.0), spotLightShininess);
        float3 specular = specularFloat * spotLightColour * spotLightSpecularStrength * specularColour;

        float spotLightDistance = distance(spotLightPosition, input.worldPos);
        float spotLightAttenuation = 1.0f / (spotLightAttenuationConstant + spotLightAttenuationLinear * spotLightDistance + spotLightAttenuationQuadratic * pow(spotLightDistance, 2.0f));

        float theta = dot(toLight, normalize(-spotLightDirection));
        float epsilon = spotLightInnerCutoff - spotLightOuterCutoff;
        float intensity = clamp((theta - spotLightOuterCutoff) / epsilon, 0.0f, 1.0f);

        //cumulativeColour += (ambient + diffuse + specular) * spotLightAttenuation;
        cumulativeColour += (ambient + diffuse + specular) * spotLightAttenuation * intensity;
    }

    float alpha = 1.0f - smoothstep(40.0f, 70.0f, distance(float3(0.0f, 0.0f, 0.0f), input.worldPos));
    float4 finalColour;
    finalColour.rgb = cumulativeColour;
    finalColour.a = alpha;
    return finalColour;
}