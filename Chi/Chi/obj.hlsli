struct VS_OUT
{
	float4 position : SV_POSITION;
	float4 color : COLOR;
	float2 texcoord : TEXCOORD;

};

struct Light
{
	float4 pos;
	float4 color;
	float4 attenuate;
};

struct lineLight
{
	float4 direction;
	float4 color;
};

struct Material
{
	float4 ambient;
	float4 diffuse;
	float4 specular;
};

cbuffer CONSTANT_BUFFER : register(b0)
{
	row_major float4x4 world_view_projection;
	row_major float4x4 world;
	float4 camPos;
	lineLight line_light;
	Light pntLight[5];
	Material material;
};