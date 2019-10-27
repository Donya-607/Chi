#include "skinned_mesh.hlsli"

float4 main(VS_OUT pin) : SV_TARGET
{
	float3 _color = (0,0,0);
	float3 V = normalize(camPos.xyz - pin.position.xyz);
	float3 L ,D,R, specularColor, ambientColor, diffuseColor;
	float A;
	ambientColor = pin.color.xyz;
	float4 _L = normalize(-line_light.direction);

	_color = ambientColor.xyz * line_light.color.xyz* max(0, dot(_L, pin.normal));
	for (int i = 0; i < 5; i++)
	{
		if (pntLight[i].pos.w == 0)
			continue;

		L = pntLight[i].pos.xyz - pin.position.xyz;
		D = length(L);
		L = normalize(L);
		R = 2.0*pin.normal*dot(pin.normal, L) - L;
		A = saturate(1.0f / (pntLight[i].attenuate.x + pntLight[i].attenuate.y*D + pntLight[i].attenuate.z*D*D));
		diffuseColor = (material.diffuse.xyz*pntLight[i].diffuse.xyz)*saturate(dot(L, pin.normal))*A;//‚½‚Ô‚ñOK
		specularColor = pow(saturate(dot(R, V)), material.specular.w)*material.specular.xyz*pntLight[i].specular.xyz*A;
		_color += saturate(diffuseColor + specularColor);
	}
	_color += saturate(ambientColor);
	return float4(saturate(_color)  ,pin.color.w);
}