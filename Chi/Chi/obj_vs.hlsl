#include "obj.hlsli"

VS_OUT main(float4 position : POSITION, float4 normal : NORMAL, float2 texcoord : TEXCOORD)
{
	VS_OUT vout;
	vout.position = mul(position, world_view_projection);

	normal.w = 0;
	vout.color = float4(1, 1, 1, 1);
	vout.texcoord = texcoord;
	return vout;

}