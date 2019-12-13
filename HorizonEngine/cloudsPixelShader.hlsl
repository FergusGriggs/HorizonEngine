
cbuffer constantBuffer : register(b0)
{
    float3 cameraPosition;
    float padding1;

    float3 lightDirection;
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

float GetDensityAt(float3 position)
{
    float3 modPos = float3(abs(fmod(position.x, 10.0f)), position.y, abs(fmod(position.z, 10.0f)));
    float dist = distance(modPos, float3(5.0f, 3.0f, 5.0f));
    return smoothstep(3.0f, 5.5f, dist);
    //return (0.5 + 0.5 * sin(position.x)) * (0.5 + 0.5 * sin(position.z)); //sin(position.y)
}

float4 main(PS_INPUT input) : SV_TARGET
{
    int sampleCount = 9;
    float oneOverSampleCount = 1.0f / (float)sampleCount;
    
    float3 mainRayDirection = normalize(input.worldPos - cameraPosition);
    
    float angle = 1.570796f - acos(dot(mainRayDirection, float3(0.0f, 1.0f, 0.0f)));
    float dist = 6.0f / sin(angle);
    float step = dist / (float)sampleCount;
    
    float density = 0.0f;
    float3 rayPos = float3(input.worldPos.x, 0.0f, input.worldPos.z);
    
    [unroll(sampleCount)]
    while(true)
    {
        density += GetDensityAt(rayPos);
        rayPos += mul(mainRayDirection, step);
    }
    
    float alpha = 1.0f - smoothstep(20.0f, 110.0f, distance(float3(input.worldPos.x, 0.0f, input.worldPos.z), float3(0.0f, 0.0f, 0.0f)));
    return float4(1.0f, 1.0f, 1.0f, alpha * (1.0f - density * oneOverSampleCount));
}