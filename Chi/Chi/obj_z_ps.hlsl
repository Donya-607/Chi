#include "obj.hlsli"
Texture2D diffuse_map : register(t0);
SamplerState diffuse_map_sample_state : register(s0);

float4 main(VS_OUT pin) : SV_TARGET
{
	float4 sampleColor = diffuse_map.Sample(diffuse_map_sample_state,pin.texcoord) * pin.color;

	float4 screen_pos = mul(pin.pos,world_view_projection);
	float pos = screen_pos.z / screen_pos.w;
	return float4(pos, pos, pos, sampleColor.w);
}