#include "sprite.hlsli"
Texture2D diffuse_map : register(t0);
SamplerState diffuse_map_sample_state : register(s0);

float4 main(VS_OUT pin) : SV_TARGET
{
	float4 color = diffuse_map.Sample(diffuse_map_sample_state,pin.texcoord);
	if (pin.color.r > color.r)
		color.r = 0;

	if (pin.color.g > color.g)
		color.g = 0;

	if (pin.color.b > color.b)
		color.b = 0;

	color.w = 1.0f;
	return color;
}