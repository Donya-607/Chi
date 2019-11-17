#include "bloom.hlsli"

Texture2D DiffuseTexture : register(t0);
SamplerState DecalSampler : register(s0);

float4 main(PSInput input) : SV_TARGET0
{
	float4 color = (float4)0;


	for (int i = 1; i < 9; i++) {
		float4 sampling_color = DiffuseTexture.Sample(DecalSampler, input.Tex[i]);
		color += sampling_color * weight[i];

	}
	//	カラーバランス調整
	//color.rgb *= screenColor.rgb;

	return color;
}
