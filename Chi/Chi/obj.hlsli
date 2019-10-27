struct VS_OUT
{
	float4 position : SV_POSITION;
	float4 color : COLOR;
	float2 texcoord : TEXCOORD;

};

struct Light
{
	float4 pos;
	float4 diffuse;
	float4 specular;
	float4 attenuate;
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
	float4 ambient;
	Light pntLight;
	Material material;
};