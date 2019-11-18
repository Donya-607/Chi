#include "filter.hlsli"

Texture2D DiffuseTexture : register(t0);
SamplerState DecalSampler : register(s0);

float4 main(PSInput input) : SV_TARGET0
{
	float4 color = DiffuseTexture.Sample(DecalSampler, input.Tex);
		
	color.rgb += status.r;

	color.rgb = ((color.rgb - 0.5f)*status.g) + 0.5f; 

	float avr = (color.r + color.g + color.b) / 3;
	color.rgb = (color.rgb - avr) * status.b + avr; 


	return color;
}
