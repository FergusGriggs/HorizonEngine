
// Quad vertex shader

#include "../shared/standard.hlsli"

VSPS_TRANSFER main(float4 pos : POSITION)
{
    VSPS_TRANSFER output;
	output.pos = pos;
	output.texCoord = float2(pos.x * 0.5f + 0.5f, -pos.y * 0.5f + 0.5f);
	return output;
}