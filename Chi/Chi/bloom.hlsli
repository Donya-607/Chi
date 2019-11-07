//--------------------------------------------
//	テクスチャ
//--------------------------------------------


//--------------------------------------------
//	グローバル変数
//--------------------------------------------
static const float3 screenColor = { 1.2,1.0,0.8 };
static const float weight[9] = {
{ 0.125,0.03125,0.0625,0.03125 },
{ 0.0625,0.0625 },
{ 0.03125,0.0625,0.03125 },

};



//--------------------------------------------
//	データーフォーマット
//--------------------------------------------

struct VSInput
{
	float4 Position : POSITION;
	float2 Tex      : TEXCOORD;
	float4 Color    : COLOR;
};

struct PSInput
{
	float4 Position : SV_POSITION;
	float4 Color : COLOR;
	float2 Tex[9]: TEXCOORD;


};

//cbuffer
cbuffer CONSTANT_BUFFER : register(b0)
{
	float screenWidth;
	float screenHeight;
	float blur;
	float dummy;
};
