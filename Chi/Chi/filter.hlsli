struct VSInput
{
	float4 Position : POSITION;
	float2 Tex      : TEXCOORD;
};

struct PSInput
{
	float4 Position : SV_POSITION;
	float2 Tex: TEXCOORD;
};

//cbuffer
cbuffer CONSTANT_BUFFER : register(b0)
{
	float4 status;//r : bright g : contrast b : saturate
};
