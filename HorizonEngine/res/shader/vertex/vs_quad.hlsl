
// Quad vertex shader

#include "../shared/standard.hlsli"

VSPS_TRANSFER main(float3 pos : POSITION)
{
    VSPS_TRANSFER output;
    output.pos = float4(pos, 1.0f);
	output.texCoord = float2(pos.x * 0.5f + 0.5f, -pos.y * 0.5f + 0.5f);
	return output;
}