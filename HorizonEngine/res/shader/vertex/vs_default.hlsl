
// Default vertex shader

#include "../shared/standard.hlsli"

VSPS_TRANSFER main (VS_INPUT input)
{
	VSPS_TRANSFER output;
    output.pos = mul(float4(input.pos, 1.0f), mul(cb_modelMatrix, cb_viewProjectionMatrix));
    output.texCoord = input.texCoord;
	output.normal = normalize(mul(float4(input.normal, 0.0f), cb_modelMatrix).xyz);
	output.worldPos = mul(float4(input.pos, 1.0f), cb_modelMatrix);

	output.tangent = normalize(mul(float4(input.tangent, 0.0f), cb_modelMatrix).xyz);
	output.bitangent = normalize(mul(float4(input.bitangent, 0.0f), cb_modelMatrix).xyz);

    return output;
}