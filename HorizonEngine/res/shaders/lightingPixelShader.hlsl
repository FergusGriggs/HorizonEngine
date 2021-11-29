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

cbuffer constantBuffer : register(b0)
{
    //PACK_SEAM
    int numPointLights;
    int numSpotLights;
    float2 padding1;

	//PACK_SEAM
    int useNormalMapping;
    int useParallaxOcclusionMapping;
    float depthScale;
    int showWorldNormals;

    //PACK_SEAM
    int showUVs;
    int cullBackNormals;
    int selfShadowing;
    int gammaCorrection;

    //PACK_SEAM
    int miscToggleA;
    int miscToggleB;
    int miscToggleC;
    int roughnessMapping;

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
    float2 texCoord : TEXCOORD;
};

SamplerState samplerState : SAMPLER : register(s0);

Texture2D albedoTexture : TEXTURE: register(t0);
Texture2D positionRoughnessTexture : TEXTURE: register(t1);
Texture2D normalAOTexture : TEXTURE: register(t2);
Texture2D emissionMetallicTexture : TEXTURE: register(t3);

float2 getParallaxTextureCoords(float3 viewDir, float2 texCoords)
{
    viewDir.y = -viewDir.y;

    const float minLayers = 8;
    const float maxLayers = 32;
    float numLayers = lerp(maxLayers, minLayers, max(0.0f, dot(float3(0.0f, 0.0f, 1.0f), viewDir)));//128.0f;

    // calculate the size of each layer
    float layerDepth = 1.0f / numLayers;
    // depth of current layer
    float currentLayerDepth = 0.0;
    // the amount to shift the texture coordinates per layer (from vector P)

    float2 P = (viewDir.xy / viewDir.z) * depthScale;
    float2 deltaTexCoords = P / numLayers;
  
    // get initial values
    float2 currentTexCoords = texCoords;
    float currentDepthMapValue = depthTexture.Sample(samplerState, currentTexCoords).r;
     
    [unroll(32)]
    while(currentLayerDepth < currentDepthMapValue)
    {
        // shift texture coordinates along direction of P
        currentTexCoords -= deltaTexCoords;
        // get depthmap value at current texture coordinates
        currentDepthMapValue = depthTexture.Sample(samplerState, currentTexCoords).r;
        // get depth of next layer
        currentLayerDepth += layerDepth;
    }
    
    // get texture coordinates before collision (reverse operations)
    float2 prevTexCoords = currentTexCoords + deltaTexCoords;

    // get depth after and before collision for linear interpolation
    float afterDepth = currentDepthMapValue - currentLayerDepth;
    float beforeDepth = depthTexture.Sample(samplerState, prevTexCoords).r - currentLayerDepth + layerDepth;
 
    // interpolation of texture coordinates
    float weight = afterDepth / (afterDepth - beforeDepth);
    float2 finalTexCoords = prevTexCoords * weight + currentTexCoords * (1.0f - weight);

    return finalTexCoords;
}

float getShadowFactorForLightAtCoord(float3 toLight, float2 texCoord, bool softShadow)
{
    toLight.y *= -1.0f;

    float shadowFactor = 0.0f;
    int minLayers = 8;
    int maxLayers = 16;

    float2 dx = ddx(texCoord);
    float2 dy = ddy(texCoord);
    float initialDepth = depthTexture.Sample(samplerState, texCoord).r;

    float lightNormalDot = dot(float3(0.0f, 0.0f, 1.0f), toLight);
    if (lightNormalDot > 0.0f)
    {
        if (depthScale < 0.0001f)
        {
            shadowFactor = 1.0f;
        }
        else
        {
            float parallaxScale = depthScale;// *initialDepth;

            float numSamplesUnderSurface = 0.0f;

            float numLayers = lerp(maxLayers, minLayers, dot(float3(0.0f, 0.0f, 1.0f), toLight));

            float2 totalDiff = (toLight.xy / toLight.z) * parallaxScale;
            float2 texStep = totalDiff / numLayers;

            float layerHeight = initialDepth / numLayers;

            float currLayerDepth = initialDepth - layerHeight;
            float2 currentTexCoord = texCoord + texStep;
            float depthFromTex = depthTexture.SampleGrad(samplerState, currentTexCoord, dx, dy).r;
            int stepIndex = 1;
            int numIter = 0;

            [unroll(16)]
            while (currLayerDepth > 0.0f)
            {
                // If under surface at sample
                if (depthFromTex < currLayerDepth)
                {
                    numSamplesUnderSurface += 1.0f;
                    float newShadowFactor = min(1.0f, (currLayerDepth - depthFromTex) * (1.0f - stepIndex / numLayers) * 5.0f);
                    shadowFactor = max(shadowFactor, newShadowFactor);
                }

                stepIndex += 1;
                currLayerDepth -= layerHeight;
                currentTexCoord += texStep;
                depthFromTex = depthTexture.SampleGrad(samplerState, currentTexCoord, dx, dy).r;
            }

            if (numSamplesUnderSurface < 1)
            {
                shadowFactor = 1.0f;
            }
            else
            {
                if (softShadow)
                {
                    shadowFactor = 1.0f - shadowFactor;
                }
                else
                {
                    shadowFactor = 0.0f;
                }
            }

            shadowFactor *= min(1.0f, lightNormalDot * 10.0f);
        }
    }

    return shadowFactor;
}

float4 main(PS_INPUT input) : SV_TARGET
{
    float3 viewDirection = normalize(cameraPosition - input.worldPos);

    float3 normal = normalize(input.normal);
    float3 tangent = normalize(input.tangent);
    float3 bitangent = normalize(input.bitangent);
    
    //float3 tangent = normalize(input.tangent - dot(input.tangent, normal) * normal);

    //float3 bitangent = normalize(cross(tangent, normal));

    float3x3 tangentToWorld = float3x3(tangent, bitangent, normal);
    float3x3 worldToTangent = transpose(tangentToWorld);

    float2 unmodifiedTexCoord = input.texCoord;
    if (useParallaxOcclusionMapping)
    {
        //float texDepth = depthTexture.Sample(samplerState, input.texCoord).r;
        //return float4(texDepth, texDepth, texDepth, 1.0f);

        float3 tangentViewDir = mul(viewDirection, worldToTangent);
        input.texCoord = getParallaxTextureCoords(tangentViewDir, input.texCoord);

        //float2 texCoordDiff = input.texCoord - unmodifiedTexCoord;
        //input.worldPos += tangent * texCoordDiff.x + bitangent * texCoordDiff.y - normal * depthTexture.Sample(samplerState, input.texCoord).r;

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

        //normalColour.g *= -1.0f;

        //normal = normalize(mul(float3(normal.x, -normal.y, normal.z), input.TBNMatrix)); //float3(0.0f, 0.5f, 0.5f)
        normal = normalize(mul(normalColour, tangentToWorld)); //float3(0.0f, 0.5f, 0.5f)

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

    float facingFactor = max(0.0f, dot(viewDirection, normal));
    //return float4(facingFactor, facingFactor, facingFactor, 1.0f);

    //if (useParallaxOcclusionMapping)
    //{
    //    //return float4(tangentProgress, bitangentProgress, 0.0f, 1.0f);
    //    float3 normalColour = normal * 0.5f + 0.5f;
    //    return float4(normalColour, 1.0f);
    //}

    float3 cumulativeColour = float3(0.0f, 0.0f, 0.0f);

    // DIRECTIONAL LIGHT
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

    // POINT LIGHT
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
    // SPOT LIGHT
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

    //return float4(fresnelFactor, fresnelFactor, fresnelFactor, 1.0f);

    //return float4(fresnelFactor, fresnelFactor, fresnelFactor, 1.0f);

    cumulativeColour = lerp(cumulativeColour, directionalLight.colour, fresnelFactor);

    if (gammaCorrection)
    {
        cumulativeColour = pow(cumulativeColour, 1.0f / 2.2f);
    }

    return float4(cumulativeColour, 1.0f);
}