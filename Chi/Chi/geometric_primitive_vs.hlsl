#include "geometric_primitive.hlsli"

VS_OUT main(float4 position : POSITION, float4 normal : NORMAL)
{
	VS_OUT vout;
	vout.position = mul(position, world_view_projection);
	vout.posw = mul(position, world);
	normal.w = 0;
	vout.normal = normalize(mul(normal, world));
	return vout;
}


	//float4 diffuseColor = (material_color.xyz * diffuse.xyz * max(0, dot(L, N)), 1.0);
	//float3 R = 2.0*N*dot(N, L) - L;
	//float3 V = normalize(camPos.xyz - vout.position);
	//float4 specularColor = ((pow(saturate(dot(R, V)), specular)*material_color.xyz*(1, 0, 0)), 1.0);
	//float4 ambientColor = ((material_color.xyz*ambient.xyz), 1.0);
	//vout.color = diffuseColor + specularColor + ambientColor;
