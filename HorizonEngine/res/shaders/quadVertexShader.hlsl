struct VS_OUTPUT
{
	float4 pos: SV_POSITION;
	float2 texCoord : TEXCOORD;
};

VS_OUTPUT main(float4 pos : POSITION)
{
	VS_OUTPUT output;
	output.pos = pos;
	output.texCoord = float2(pos.x * 0.5f + 0.5f, -pos.y * 0.5f + 0.5f);
	return output;
}