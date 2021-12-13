
#ifndef __PARALLAX_OCCLUSION_HEADER_HLSL__
#define __PARALLAX_OCCLUSION_HEADER_HLSL__

SamplerState LinearWrapSampleState
{
	Filter = MIN_MAG_MIP_LINEAR;
	AddressU = Wrap;
	AddressV = Wrap;
};

float2 getParallaxTextureCoords(float3 viewDir, float2 texCoords, Texture2D depthTexture, float depthScale)
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
	float currentDepthMapValue = depthTexture.Sample(LinearWrapSampleState, currentTexCoords).r;
    
    [unroll(32)]
    while (currentLayerDepth < currentDepthMapValue)
    {
        // shift texture coordinates along direction of P
        currentTexCoords -= deltaTexCoords;
        // get depthmap value at current texture coordinates
		currentDepthMapValue = depthTexture.Sample(LinearWrapSampleState, currentTexCoords).r;
        // get depth of next layer
        currentLayerDepth += layerDepth;
    }

    // get texture coordinates before collision (reverse operations)
    float2 prevTexCoords = currentTexCoords + deltaTexCoords;

    // get depth after and before collision for linear interpolation
    float afterDepth = currentDepthMapValue - currentLayerDepth;
	float beforeDepth = depthTexture.Sample(LinearWrapSampleState, prevTexCoords).r - currentLayerDepth + layerDepth;

    // interpolation of texture coordinates
    float weight = afterDepth / (afterDepth - beforeDepth);
    float2 finalTexCoords = prevTexCoords * weight + currentTexCoords * (1.0f - weight);

    return finalTexCoords;
}

float getShadowFactorForLightAtCoord(float3 toLight, float2 texCoord, bool softShadow, Texture2D depthTexture, float depthScale)
{
    toLight.y *= -1.0f;

    float shadowFactor = 0.0f;
    int minLayers = 8;
    int maxLayers = 16;

    float2 dx = ddx(texCoord);
    float2 dy = ddy(texCoord);
	float initialDepth = depthTexture.Sample(LinearWrapSampleState, texCoord).r;

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
			float depthFromTex = depthTexture.SampleGrad(LinearWrapSampleState, currentTexCoord, dx, dy).r;
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
				depthFromTex = depthTexture.SampleGrad(LinearWrapSampleState, currentTexCoord, dx, dy).r;
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

#endif //__PARALLAX_OCCLUSION_HEADER_HLSL__
