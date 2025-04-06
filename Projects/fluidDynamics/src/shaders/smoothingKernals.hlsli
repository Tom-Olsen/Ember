#ifndef __INCLUDE_GUARD_smoothingKernals_hlsli__
#define __INCLUDE_GUARD_smoothingKernals_hlsli__
#include "math.hlsli"



float SmoothingKernal_Poly6(float r, float h)
{
    const float c0 = 315.0f / (64.0f * math_PI);
    float c = c0 / pow(h, 9.0f);
    return (r < h) ? c * pow(h * h - r * r, 3.0f) : 0.0f;
}



#endif //__INCLUDE_GUARD_smoothingKernals_hlsli__