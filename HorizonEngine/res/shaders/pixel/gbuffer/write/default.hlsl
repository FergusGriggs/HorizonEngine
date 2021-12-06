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