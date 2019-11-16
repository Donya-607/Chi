#include "geometric_primitive.hlsli"

float4 main(VS_OUT pin) : SV_TARGET
{
	float pos = pin.position.z;
	return float4(pos,pos,pos, material.ambient.w);
}
