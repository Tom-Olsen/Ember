#ifndef __INCLUDE_GUARD_smoothingKernals_h__
#define __INCLUDE_GUARD_smoothingKernals_h__
#include "emberMath.h"


namespace smoothingKernals
{
    // Used for almost all calculations.
    inline float Poly6(float r, float h)
    {
        constexpr float c0 = 315.0f / (64.0f * math::pi);
        float c = c0 / math::Pow(h, 9.0f);
        return (r < h) ? c * math::Pow(h * h - r * r, 3.0f) : 0.0f;
    }
    inline Float2 DPoly6(float r, const Float2& dir, float h)
    {
        constexpr float c0 = 315.0f / (64.0f * math::pi);
        float c = c0 / math::Pow(h, 9.0f);
        return (r < h) ? -6.0f * c * math::Pow(h * h - r * r, 2.0f) * dir : 0.0f;
    }
    inline float DDPoly6(float r, float h)
    {
        constexpr float c0 = 315.0f / (64.0f * math::pi);
        float c = c0 / math::Pow(h, 9.0f);
        float r2 = r * r;
        float h2r2 = h * h - r2;
        return (r < h) ? 6.0f * h2r2 * c * (4.0f * r2 - 2.0f * h2r2) : 0.0f;
    }

    // Used for pressure calculations.
    inline float Spiky(float r, float h)
    {
        constexpr float c0 = 15.0f / math::pi;
        float c = c0 / math::Pow(h, 6.0f);
        return (r < h) ? c * math::Pow(h - r, 3.0f) : 0.0f;
    }
    inline Float2 DSpiky(float r, const Float2& dir, float h)
    {
        constexpr float c0 = 15.0f / math::pi;
        float c = c0 / math::Pow(h, 6.0f);
        return (r < h) ? -3.0f * c * math::Pow(h - r, 2.0f) * dir : 0.0f;
    }
    inline float DDSpiky(float r, float h)
    {
        constexpr float c0 = 15.0f / math::pi;
        float c = c0 / math::Pow(h, 6.0f);
        float hr = h - r;
        return (r < h) ? 3.0 * c * hr * (2.0f - hr / r) : 0.0f;
    }
    
    // Used for viscosity calculations:
    inline float Viscos(float r, float h)
    {
        constexpr float c0 = 15.0f / (2.0f * math::pi);
        float c = c0 / math::Pow(h, 3.0f);
        return (r < h) ? c * (-0.5f * math::Pow(r / h, 3.0f) + (r * r) / (h  * h) + 0.5f * h / r - 1.0f) : 0.0f;
    }
    inline Float2 DViscos(float r, const Float2& dir, float h)
    {
        constexpr float c0 = 15.0f / (2.0f * math::pi);
        float c = c0 / math::Pow(h, 3.0f);
        float r2 = r * r;
        float h2 = h * h;
        return (r < h) ? c * (-1.5f * r2 / (h2 * h) + 2.0f * r / h2 - 0.5f * h / r2) * dir : 0.0f;
    }
    inline float DDViscos(float r, float h)
    {
        constexpr float c0 = 15.0f / (2.0f * math::pi);
        float h2 = h * h;
        float h3 = h2 * h;
        float c = c0 / h3;
        return (r < h) ? c * (4.0f / h2 + 0.5f * h / (r * r * r) - 4.5f * r / h3) : 0.0f;
    }
};



#endif // __INCLUDE_GUARD_smoothingKernals_h__