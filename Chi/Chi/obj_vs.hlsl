#include "obj.hlsli"

VS_OUT main(float4 position : POSITION, float4 normal : NORMAL, float2 texcoord : TEXCOORD)
{
	//VS_OUT vout;
	//vout.position = mul(position, world_view_projection);

	//normal.w = 0;
	//float4 N = normalize(mul(normal, world));
	//float4 L = normalize(-light_direction);
	//float4 diffuseColor = (material_color.xyz * diffuse.xyz * max(0, dot(L, N)),1.0);
	//float3 R = 2.0*N*dot(N, 1) - 1;
	//float3 V = normalize(camPos.xyz - vout.position);
	//float4 specularColor = ((pow(saturate(dot(R, V)), specular.w)*material_color.xyz*( 1,0,0)), 1.0);
	//float4 ambientColor = (material_color.xyz*ambient.xyz, 1.0);
	//vout.color = diffuseColor + specularColor + ambientColor;
	//vout.color.a = material_color.a;
	//vout.texcoord = texcoord;
	//return vout;
	VS_OUT vout;
	vout.position = mul(position, world_view_projection);
	//vout.position = position;
	normal.w = 0;
	//vout.normal = normalize(mul(normal, world));
	vout.texcoord = texcoord;
	return vout;

}