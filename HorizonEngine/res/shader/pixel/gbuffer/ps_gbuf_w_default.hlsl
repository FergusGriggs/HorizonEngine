
#include "../../shared/standard.hlsli"
#include "../../shared/scene.hlsli"
#include "../../shared/POM.hlsli"

struct PS_OUTPUT
{
    float4 albedo : SV_TARGET0;
    float4 positionRoughness : SV_TARGET1;
    float4 normalAO : SV_TARGET2;
    float4 emissionMetallic : SV_TARGET3;
};

SamplerState samplerState : SAMPLER : register(s0);

Texture2D albedoTexture : TEXTURE : register(t0);
Texture2D roughnessTexture : TEXTURE : register(t1);
Texture2D normalTexture : TEXTURE : register(t2);
Texture2D metallicTexture : TEXTURE : register(t3);
Texture2D emissionTexture : TEXTURE : register(t4);
Texture2D depthTexture : TEXTURE : register(t5);

Texture2D ambientOcclusionTexture : TEXTURE : register(t6);

PS_OUTPUT main(VSPS_TRANSFER input)
{
    PS_OUTPUT output;

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
	
    // Output albedo
    output.albedo = albedoTexture.Sample(samplerState, input.texCoord);
    
    // Output position
    output.positionRoughness.rgb = input.worldPos;

    // Output roughness
	output.positionRoughness.a = roughnessTexture.Sample(samplerState, input.texCoord).r;

	if (cb_useNormalMapping)
	{
		float3 normalColour = normalTexture.Sample(samplerState, input.texCoord).rgb;

		if (cb_miscToggleB)
		{
			output.albedo.rgb = normalColour;
		}

		normalColour = normalize(normalColour * 2.0f - 1.0f);
		normal = normalize(mul(normalColour, tangentToWorld));

		if (cb_cullBackNormals && dot(viewDirection, normal) < 0.0f)
			discard;
	}
	
    // Output normal
    output.normalAO.rgb = normal;

    // Output ambient occlusion
    output.normalAO.a = 1.0f; //ambientOcclusionTexture.Sample(samplerState, input.texCoord).r;

    // Output emission
    output.emissionMetallic.rgb = emissionTexture.Sample(samplerState, input.texCoord).rgb;

    // Output metallic
    output.emissionMetallic.a = 1.0f; //metallicTexture.Sample(samplerState, input.texCoord).r;

    return output;
}