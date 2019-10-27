#include "geometric_primitive.hlsli"
float3 Diffuse(float3 N, float3 L, float3 C, float3 K, float A)
{
	float D = dot(N, L);
	D = max(0, D);			// 負の値を０にする
	return (K * C) * (D*A);
}

float3 BlinnPhong(float3 N, float3 L, float3 C, float3 V,
	float3 K, float Power, float A)
{
	//ハーフベクトル
	float3 H = 2.0*N*dot(N, L) - L;

	float3 S = dot(H, V);
	S = max(0, S);
	S = pow(S, Power);
	S = S*K*C*A;
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
	S = S*K*C;
	return S;
}



float4 main(VS_OUT pin) : SV_TARGET
{
	float3 color = (0,0,0);
	float3 _color = (0, 0, 0);
	float3 N = pin.normal.xyz;
	float3 V = normalize(camPos.xyz - pin.posw.xyz);
	float3 L ,D, specularColor, diffuseColor;
	float A;
	float4 _L = normalize(-line_light.direction);


	A = max(0, dot(_L, pin.normal));

	_color = material.ambient.xyz;
	color = material.ambient.xyz *(line_light.color.xyz* max(0, dot(_L, pin.normal)));


	for (int i = 0; i < 5; i++)
	{
		if (pntLight[i].pos.w == 0)
			continue;

		L = pntLight[i].pos.xyz - pin.posw.xyz;
		D = length(L);
		L = normalize(L);
		A = saturate(1.0f / (pntLight[i].attenuate.x + pntLight[i].attenuate.y*D + pntLight[i].attenuate.z*D*D));

		diffuseColor = Diffuse(N,L,pntLight[i].color,
			material.diffuse.xyz,A);//たぶんOK

		specularColor = BlinnPhong(N,L, pntLight[i].color.xyz,
			V, material.specular.xyz,material.specular.w,A);

		_color += (diffuseColor + specularColor);
	}
	color += _color;
	return float4(saturate(color) ,material.ambient.w);
}