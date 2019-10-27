#include "shadow_first.hlsli"



float4 main(VS_OUT pin) : SV_TARGET
{

	float z = pin.position.z/pin.position.w;
return float4(z,z,z,1.0f);
}