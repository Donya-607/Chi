#include "skinned_mesh.hlsli"

VS_OUT main(float4 position : POSITION, float4 normal : NORMAL, float2 texcoord : TEXCOORD, float4 bone_weight : WEIGHTS, uint4 bone_indices : BONES)
{
	VS_OUT vout;
	vout.position = mul(position, world_view_projection);
	vout.posw = mul(position, world);

	normal.w = 0;
	vout.normal = normalize(mul(normal, world));
	vout.color = material.ambient;
	vout.texcoord = texcoord;
	return vout;
}