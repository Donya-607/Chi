#include "obj.hlsli"
Texture2D diffuse_map : register(t0);
SamplerState diffuse_map_sample_state : register(s0);
Texture2D z_map : register(t1);
SamplerState z_sample_state : register(s1);


float4 main(VS_OUT pin) : SV_TARGET
{

	float4 color = diffuse_map.Sample(diffuse_map_sample_state,pin.texcoord) * pin.color;

	if (color.x <= judge_color.x)
		color.x = 0;
	if (color.y <= judge_color.y)
		color.y = 0;
	if (color.z <= judge_color.z)
		color.z = 0;
	return color;

}