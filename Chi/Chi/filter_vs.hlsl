#include "filter.hlsli"

PSInput main(VSInput input)
{
	PSInput output = (PSInput)0;

	// o—Í’lİ’è.
	output.Position = input.Position;

	output.Tex = input.Tex;

	return output;
}
