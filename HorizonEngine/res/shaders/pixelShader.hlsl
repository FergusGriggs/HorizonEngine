//pixelShader.hlsl
//Default pixel shader

struct DirectionalLight
{
    float3 colour;
    float ambientStrength;

    float3 direction;
    float padding1;
};

struct PointLight
{
    float3 position;
    float padding1;
    
    float3 colour;
    float padding2;
    
    float attenuationConstant;
    float attenuationLinear;
    float attenuationQuadratic;
    float padding3;
};

struct SpotLight
{
    float3 position;
    float padding1;
    
    float3 colour;
    float padding2;
    
    float attenuationConstant;
    float attenuationLinear;
    float attenuationQuadratic;
    float innerCutoff;
    
    float outerCutoff;
    float3 direction;
};

struct Material
{
    float shininess;
    float specularity;
    float2 padding1;
};

cbuffer constantBuffer : register(b0)
{
    //PACK_SEAM
    int numPointLights;
    int numSpotLights;
    float2 padding1;

	//PACK_SEAM
    int useNormalMapping;
    int useParallaxOcclusionMapping;
    float parallaxOcclusionMappingHeight;
    int fresnel;

	//PACK_SEAM
    Material objectMaterial;

	//PACK_SEAM
    DirectionalLight directionalLight;
    PointLight pointLights[10];
    SpotLight spotLights[20];

	//PACK_SEAM
    float3 cameraPosition;
    float padding2;
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

    if (useNormalMapping)
        {
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
        float3 ambient = directionalLight.colour * directionalLight.colour * textureColour;

        float3 toLight = -directionalLight.direction;

        float diffuseFloat = max(dot(toLight, normal), 0.0f);
        float3 diffuse = diffuseFloat * directionalLight.colour * textureColour;

        float3 reflectDirection = reflect(-toLight, normal);

        float specularFloat = pow(max(dot(viewDirection, reflectDirection), 0.0), objectMaterial.shininess);
        float3 specular = specularFloat * directionalLight.colour * objectMaterial.specularity * specularColour;

        cumulativeColour += ambient + diffuse + specular;
    }

    // POINT LIGHT
    {
        for (int i = 0; i < numPointLights; ++i)
        {
            float3 toLight = normalize(pointLights[i].position- input.worldPos);

            float diffuseFloat = max(dot(toLight, normal), 0.0f);
            float3 diffuse = diffuseFloat * pointLights[i].colour * textureColour;

            float3 reflectDirection = reflect(-toLight, normal);

            float specularFloat = pow(max(dot(viewDirection, reflectDirection), 0.0), objectMaterial.shininess);
            float3 specular = specularFloat * pointLights[i].colour * specularColour;

            float pointLightDistance = distance(pointLights[i].position, input.worldPos);
            float pointLightAttenuation = 1.0f / (pointLights[i].attenuationConstant + pointLights[i].attenuationLinear * pointLightDistance + pointLights[i].attenuationQuadratic * pow(pointLightDistance, 2.0f));

            cumulativeColour += (diffuse + specular) * pointLightAttenuation;
        }
    }
    // SPOT LIGHT
    {
        for (int i = 0; i < numSpotLights; ++i)
        {
            float3 toLight = normalize(spotLights[i].position- input.worldPos);

            float diffuseFloat = max(dot(toLight, normal), 0.0f);
            float3 diffuse = diffuseFloat * spotLights[i].colour* textureColour;

            float3 reflectDirection = reflect(-toLight, normal);

            float specularFloat = pow(max(dot(viewDirection, reflectDirection), 0.0), objectMaterial.shininess);
            float3 specular = specularFloat * spotLights[i].colour * specularColour;

            float spotLightDistance = distance(spotLights[i].position, input.worldPos);
            float spotLightAttenuation = 1.0f / (spotLights[i].attenuationConstant + spotLights[i].attenuationLinear * spotLightDistance + spotLights[i].attenuationQuadratic * pow(spotLightDistance, 2.0f));

            float lightAngle = (acos(dot(-toLight, spotLights[i].direction)) * 180.0) / 3.141592f;
            float lightCutoffAmount = 1.0 - smoothstep(spotLights[i].innerCutoff, spotLights[i].outerCutoff, lightAngle);

            //cumulativeColour += (ambient + diffuse + specular) * spotLightAttenuation;
            cumulativeColour += (diffuse + specular) * spotLightAttenuation * lightCutoffAmount;
        }
    }

    float alpha = 1.0f; //1.0f - smoothstep(75.0f, 85.0f, distance(float3(0.0f, 0.0f, 0.0f), input.worldPos));
    if (fresnel)
    {
        alpha = alpha * (1.0f - 0.4f * dot(normal, viewDirection));
    }
    float4 finalColour;
    finalColour.rgb = cumulativeColour;
    finalColour.a = alpha;
    return finalColour;
}