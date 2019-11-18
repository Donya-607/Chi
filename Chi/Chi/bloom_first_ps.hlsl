#include "bloom.hlsli"

Texture2D DiffuseTexture : register(t0);
SamplerState DecalSampler : register(s0);

float4 main(PSInput input) : SV_TARGET0
{
	float4 color = DiffuseTexture.Sample(DecalSampler, input.Tex[0]);

		//	�J���[�o�����X����
		//color.rgb *= screenColor.rgb;

		return color;
}
