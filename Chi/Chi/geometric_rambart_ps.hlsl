#include "geometric_primitive.hlsli"

float4 main(VS_OUT pin) : SV_TARGET
{
	float3 color = (0,0,0);
	float3 V = normalize(camPos.xyz - pin.posw.xyz);
	float3 L, D, R, specularColor, ambientColor, diffuseColor;
	float A;
	//ambientColor = material.ambient.xyz*ambient.xyz;
	for (int i = 0;i < 5;i++)
	{
		if (pntLight[i].pos.w == 0)
			continue;
	L = pntLight[i].pos.xyz - pin.posw.xyz;
	D = length(L);
	L = normalize(L);
	R = max(dot(pin.normal, L),0.0f)*0.5f + 0.5f;
	A = saturate(1.0f / (pntLight[i].attenuate.x + pntLight[i].attenuate.y*D + pntLight[i].attenuate.z*D*D));
	diffuseColor = (3.0f/(4.0f*3.1415f))*(R*R*material.specular.w)*pntLight[i].diffuse.xyz * material.diffuse.xyz;
	specularColor = pow(saturate(dot(R,V)),material.specular.w)*material.specular.xyz*pntLight[i].specular.xyz*A;
	color += saturate(diffuseColor/* + specularColor + ambientColor*/);
	}

	return float4(color ,1.0);
}