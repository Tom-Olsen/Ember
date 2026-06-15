#ifndef __INCLUDE_GUARD_smoothingKernals2d_hlsli__
#define __INCLUDE_GUARD_smoothingKernals2d_hlsli__
#include "math.hlsli"



// Some gradients and laplacians are singular at r=0.
// The caller must implement a fallback strategy for r<epsilon, for their choice of epsilon.



// Used for almost all calculations.
float SmoothingKernal_Poly6(float r, float h)
{
    float c0 = 4.0f / math_PI;
    float c = c0 / pow(h, 8.0f);
    return (r < h) ? c * pow(h * h - r * r, 3.0f) : 0.0f;
}
float2 SmoothingKernal_DPoly6(float r, float2 dir, float h)
{
    float c0 = 4.0f / math_PI;
    float c = c0 / pow(h, 8.0f);
    return (r < h) ? -6.0f * c * pow(h * h - r * r, 2.0f) * r * dir : float2(0, 0);
}
float SmoothingKernal_DDPoly6(float r, float h)
{
    float c0 = 4.0f / math_PI;
    float c = c0 / pow(h, 8.0f);
    float r2 = r * r;
    float h2r2 = h * h - r2;
    return (r < h) ? -12.0f * c * h2r2 * (h * h - 3.0f * r2) : 0.0f;
}



// Used for pressure calculations.
float SmoothingKernal_Spiky2(float r, float h)
{
    float c0 = 6.0f / math_PI;
    float c = c0 / pow(h, 4.0f);
    return (r < h) ? c * pow(h - r, 2.0f) : 0.0f;
}
float2 SmoothingKernal_DSpiky2(float r, float2 dir, float h)
{
    // singular at r = 0.
    float c0 = 6.0f / math_PI;
    float c = c0 / pow(h, 4.0f);
    return (r < h) ? -2.0f * c * (h - r) * dir : float2(0, 0);
}
float SmoothingKernal_DDSpiky2(float r, float h)
{
    // singular at r = 0.
    float c0 = 6.0f / math_PI;
    float c = c0 / pow(h, 4.0f);
    return (r < h) ? 2.0f * c * (2.0f - h / r) : 0.0f;
}
float SmoothingKernal_Spiky3(float r, float h)
{
    float c0 = 10.0f / math_PI;
    float c = c0 / pow(h, 5.0f);
    return (r < h) ? c * pow(h - r, 3.0f) : 0.0f;
}
float2 SmoothingKernal_DSpiky3(float r, float2 dir, float h)
{
    // singular at r = 0.
    float c0 = 10.0f / math_PI;
    float c = c0 / pow(h, 5.0f);
    return (r < h) ? -3.0f * c * pow(h - r, 2.0f) * dir : float2(0, 0);
}
float SmoothingKernal_DDSpiky3(float r, float h)
{
    // singular at r = 0.
    float c0 = 10.0f / math_PI;
    float c = c0 / pow(h, 5.0f);
    float hr = h - r;
    return (r < h) ? 3.0f * c * hr * (3.0f - h / r) : 0.0f;
}



// Used for viscosity calculations:
float SmoothingKernal_Viscos(float r, float h)
{
    // singular at r = 0.
    float c0 = 10.0f / (3.0f * math_PI);
    float c = c0 / pow(h, 2.0f);
    return (r < h) ? c * (-0.5f * pow(r / h, 3.0f) + (r * r) / (h * h) + 0.5f * h / r - 1.0f) : 0.0f;
}
float2 SmoothingKernal_DViscos(float r, float2 dir, float h)
{
    // singular at r = 0.
    float c0 = 10.0f / (3.0f * math_PI);
    float c = c0 / pow(h, 2.0f);
    float r2 = r * r;
    float h2 = h * h;
    return (r < h) ? c * (-1.5f * r2 / (h2 * h) + 2.0f * r / h2 - 0.5f * h / r2) * dir : float2(0, 0);
}
float SmoothingKernal_DDViscos(float r, float h)
{
    // singular at r = 0.
    float c0 = 10.0f / (3.0f * math_PI);
    float h2 = h * h;
    float h3 = h2 * h;
    float c = c0 / h2;
    return (r < h) ? c * (4.0f / h2 + 0.5f * h / (r * r * r) - 4.5f * r / h3) : 0.0f;
}



#endif //__INCLUDE_GUARD_smoothingKernals2d_hlsli__