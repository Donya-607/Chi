#include "skinned_mesh.hlsli"

VS_OUT main(float4 position : POSITION, float4 normal : NORMAL, float2 texcoord : TEXCOORD, float4 bone_weight : WEIGHTS, uint4 bone_indices : BONES)
{
	VS_OUT vout;

	normal.w = 0;
	float4 influence = { 0,0,0,1 };
	float3 p = { 0,0,0 };
	float3 n = { 0,0,0 };
	for (int i = 0; i < 4; i++)
	{
		float weight = bone_weight[i];
		if (weight > 0.0f)
		{
			switch (bone_indices[i])
			{
			case 0:
				influence.r = weight;
				break;
			case 1:
				influence.g = weight;
				break;
			case 2:
				influence.b = weight;
				break;
			}
		}
		p += (bone_weight[i] * mul(position, bone_transforms[bone_indices[i]])).xyz;
		n += (bone_weight[i] * mul(float4(normal.xyz, 0), bone_transforms[bone_indices[i]])).xyz;
	}

	position = float4(p, 1.0f);
	normal = float4(n, 0.0f);
	vout.position = mul(position, world_view_projection);
	vout.posw = mul(position, world);
	vout.normal = normalize(mul(normal, world));


	vout.color = material.ambient;
	vout.texcoord = texcoord;
	return vout;
}