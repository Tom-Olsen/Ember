#ifndef __INCLUDE_GUARD_smoothingKernals3d_hlsli__
#define __INCLUDE_GUARD_smoothingKernals3d_hlsli__
#include "math.hlsli"



// Used for almost all calculations.
float SmoothingKernal_Poly6(float r, float h)
{
    float c = 315.0f / (64.0f * math_PI * pow(h, 8.0f));
    return (r < h) ? c * pow(h * h - r * r, 3.0f) : 0.0f;
}
float3 SmoothingKernal_DPoly6(float r, float3 dir, float h)
{
    float c = 315.0f / (64.0f * math_PI * pow(h, 8.0f));
    return (r < h) ? -6.0f * c * pow(h * h - r * r, 2.0f) * dir : float3(0, 0, 0);
}
float SmoothingKernal_DDPoly6(float r, float h)
{
    float c = 315.0f / (64.0f * math_PI * pow(h, 8.0f));
    float r2 = r * r;
    float h2r2 = h * h - r2;
    return (r < h) ? 6.0f * c * h2r2 * (4.0f * r2 - 3.0f * h2r2) : 0.0f;
}



// Used for pressure calculations.
float SmoothingKernal_Spiky(float r, float h)
{
    float c = 15.0f / (math_PI * pow(h, 6.0f));
    return (r < h) ? c * pow(h - r, 3.0f) : 0.0f;
}
float3 SmoothingKernal_DSpiky(float r, float3 dir, float h)
{
    float c = 15.0f / (math_PI * pow(h, 6.0f));
    return (r < h) ? -3.0f * c * pow(h - r, 2.0f) * dir : float3(0, 0, 0);
}
float SmoothingKernal_DDSpiky(float r, float h)
{
    float c = 15.0f / (math_PI * pow(h, 6.0f));
    float hr = h - r;
    return (r < h) ? 6.0 * c * hr * (1.0f - hr / r) : 0.0f;
}
    


// Used for viscosity calculations:
float SmoothingKernal_Viscos(float r, float h)
{
    float c = 15.0f / (2.0f * math_PI * h * h * h);
    return (r < h) ? c * (-0.5f * pow(r / h, 3.0f) + (r * r) / (h * h) + 0.5f * h / r - 1.0f) : 0.0f;
}
float3 SmoothingKernal_DViscos(float r, float3 dir, float h)
{
    float r2 = r * r;
    float h2 = h * h;
    float h3 = h2 * h;
    float c = 15.0f / (2.0f * math_PI * h3);
    return (r < h) ? c * (-1.5f * r2 / h3 + 2.0f * r / h2 - 0.5f * h / r2) * dir : float3(0, 0, 0);
}
float SmoothingKernal_DDViscos(float r, float h)
{
    float h2 = h * h;
    float h3 = h2 * h;
    float c = 15.0f / (2.0f * math_PI * h3);
    return (r < h) ? 6.0f * c * (h - r) / h3 : 0.0f;
}



#endif //__INCLUDE_GUARD_smoothingKernals3d_hlsli__