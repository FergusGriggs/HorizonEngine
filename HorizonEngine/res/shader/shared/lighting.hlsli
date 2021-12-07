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

cbuffer lightingCBuffer : reguster(b1)
{
    DirectionalLight directionalLight;
    PointLight pointLights[10];
    SpotLight spotLights[20];

    int    numPointLights;
    int    numSpotLights;
    float2 padding1;
}
