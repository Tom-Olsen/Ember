#ifndef __INCLUDE_GUARD_mathRay_hlsli__
#define __INCLUDE_GUARD_mathRay_hlsli__



struct math_WorldSpaceRay
{
	float3 origin;
	float3 direction;
}
struct math_ScreenSpaceRay
{
	float2 origin;
	float2 direction
}



#endif // __INCLUDE_GUARD_mathRay_hlsli__