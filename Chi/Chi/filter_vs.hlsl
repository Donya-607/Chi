#include "filter.hlsli"

PSInput main(VSInput input)
{
	PSInput output = (PSInput)0;

	// 出力値設定.
	output.Position = input.Position;

	output.Tex = input.Tex;

	return output;
}
