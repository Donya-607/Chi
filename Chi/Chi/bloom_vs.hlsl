#include "bloom.hlsli"

PSInput main(VSInput input)
{
	PSInput output = (PSInput)0;

	// o—Í’lİ’è.
	output.Position = input.Position;
	output.Color = input.Color;
	output.Tex[0] = input.Tex;
	float dw = 1 / screenWidth;
	float dh = 1 / screenHeight;
#if 1
	output.Tex[1] = input.Tex + float2(-dw, -dh) * blur;
	output.Tex[2] = input.Tex + float2(0.0, -dh) * blur;
	output.Tex[3] = input.Tex + float2(+dw, -dh) * blur;
	output.Tex[4] = input.Tex + float2(-dw, 0.0) * blur;
	output.Tex[5] = input.Tex + float2(+dw, 0.0) * blur;
	output.Tex[6] = input.Tex + float2(-dw, +dh) * blur;
	output.Tex[7] = input.Tex + float2(0.0, +dh) * blur;
	output.Tex[8] = input.Tex + float2(+dw, +dh) * blur;
#endif


	return output;
}
