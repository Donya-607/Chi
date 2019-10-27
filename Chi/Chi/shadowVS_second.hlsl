#include "shadow_first.hlsli"

VS_OUT main(float4 position : POSITION, float4 normal : NORMAL)
{
	VS_OUT vout;
	vout.position = mul(position, world_view_projection);
	normal.w = 0;
	vout.normal = normalize(mul(normal, world));
	return vout;
}
