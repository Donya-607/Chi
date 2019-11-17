#include "skinned_mesh.hlsli"
float3 Diffuse(float3 N, float3 L, float3 C, float3 K, float A)
{
	float D = dot(N, L);
	D = max(0, D);			// 負の値を０にする
	return (K * C) * (D * A);
}

float3 HalfLambart(float3 N, float3 L, float3 C, float3 K, float A)
{
	float D = dot(N, L) * 0.5 + 0.5;
	D *= D;
	return (K * C) * (D * A);
}
float3 HalfLambart_L(float3 N, float3 L, float3 C, float3 K)
{
	float D = dot(N, L) * 0.5 + 0.5;
	D *= D;
	return (K * C) * D;
}

float3 Diffuse_test(float3 C, float3 K)
{
	return (K * C);
}


float3 BlinnPhong(float3 N, float3 L, float3 C, float3 V,
	float3 K, float Power, float A)
{
	//ハーフベクトル
	float3 H = 2.0 * N * dot(N, L) - L;

	float3 S = dot(H, V);
	S = max(0, S);
	S = pow(S, Power);
	S = S * K * C * A;
	return S;
}
float3 BlinnPhong_L(float3 N, float3 L, float3 C, float3 V,
	float3 K, float Power)
{
	//ハーフベクトル
	float3 H = 2.0 * N * dot(N, L) - L;

	float3 S = dot(H, V);
	S = max(0, S);
	S = pow(S, Power);
	S = S * K * C;
	return S;
}


float3 Phong(float3 N, float3 L, float3 C, float3 E,
	float3 K, float Power)
{
	float3 R = reflect(L, N);
	R = normalize(R);
	float3 S = dot(R, E);
	S = max(0, S);
	S = pow(S, Power);
	S = S * K * C;
	return S;
}


Texture2D diffuse_map : register(t0);
SamplerState diffuse_map_sample_state : register(s0);
Texture2D z_map : register(t1);
SamplerState z_sample_state : register(s1);


float4 main(VS_OUT pin) : SV_TARGET
{
	float2 texpos = float2((pin.position.x / screen_size.x),(pin.position.y / screen_size.y));
	float z = z_map.Sample(z_sample_state, texpos).x;

	float4 screen_pos = mul(pin.pos, world_view_projection);
	float pos = screen_pos.z / screen_pos.w;
	if (z < pos)
		return float4(0,0,0, 0);

	float3 color = float3(0,0,0);
	float3 _color = float3(0, 0, 0);
	float3 N = pin.normal.xyz;
	float3 V = normalize(camPos.xyz - pin.posw.xyz);
	float3 L , specularColor, diffuseColor;
	float A ,D;
	float4 sampleColor = diffuse_map.Sample(diffuse_map_sample_state, pin.texcoord);
	float4 _L = normalize(-line_light.direction);
	color = sampleColor.xyz;

	_color = Diffuse_test(line_light.color.xyz, material.diffuse.xyz);
	//BlinnPhong_L(N, _L, line_light.color.xyz,V, material.specular.xyz, material.specular.w);


for (int i = 0; i < 5; i++)
{
	if (pntLight[i].pos.w == 0)
		continue;

	L = pntLight[i].pos.xyz - pin.posw.xyz;
	D = length(L);
	L = normalize(L);
	A = saturate(1.0f / (pntLight[i].attenuate.x + pntLight[i].attenuate.y * D + pntLight[i].attenuate.z * D * D));

	diffuseColor = HalfLambart(N, L, pntLight[i].color.xyz,
		material.diffuse.xyz, A);//たぶんOK

	specularColor = BlinnPhong(N, L, pntLight[i].color.xyz,
		V, material.specular.xyz, material.specular.w, A);

	_color += (diffuseColor + specularColor);
}
color *= _color;
if (color.x <= judge_color.x)
	color.x = 0;
if (color.y <= judge_color.y)
	color.y = 0;
if (color.z <= judge_color.z)
	color.z = 0;
color.xyz *= judge_color.w;
return float4(saturate(color) ,pin.color.w * sampleColor.w);
}