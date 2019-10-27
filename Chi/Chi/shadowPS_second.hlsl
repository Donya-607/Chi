#include "shadow_first.hlsli"

float4 main(VS_OUT pin) : SV_TARGET
{
	return float4(pin.position.z,pin.position.z,pin.position.z ,1.0);
}