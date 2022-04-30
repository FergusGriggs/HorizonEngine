
// Default vertex shader

#include "../shared/standard.hlsli"

VSPS_TRANSFER main (VS_INPUT input)
{
	VSPS_TRANSFER output;
    output.worldPos = mul(float4(input.pos, 1.0f), cb_modelMatrix);
    output.pos = mul(float4(output.worldPos, 1.0f), cb_viewProjectionMatrix);
    output.texCoord = input.texCoord;
	output.normal = normalize(mul(float4(input.normal, 0.0f), cb_modelMatrix).xyz);
	
	//output.worldPos = mul(float4(input.pos, 1.0f), cb_modelMatrix);
	//output.pos = mul(float4(input.pos, 1.0f), mul(cb_modelMatrix, cb_viewProjectionMatrix));
	/***********************************************

	MARKING SCHEME: Normal Mapping,	Basic Parallax Mapping and Parallax Occlusion Mapping with self shadowing

	DESCRIPTION: The extra tangent space information is transformed from model space to world space, normalised
	and passed through to any pixel shader stage.

	COMMENT INDEX: 2
	
	***********************************************/
	
	output.tangent = normalize(mul(float4(input.tangent, 0.0f), cb_modelMatrix).xyz);
	output.bitangent = normalize(mul(float4(input.bitangent, 0.0f), cb_modelMatrix).xyz);

    return output;
}