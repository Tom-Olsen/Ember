#pragma once
#include "emberMath.h"



namespace smoothingKernals
{
    // Some gradients and laplacians are singular at r=0.
    // The caller must implement a fallback strategy for r<epsilon, for their choice of epsilon.

    // Used for almost all calculations.
    inline float Poly6(float r, float h)
    {
        constexpr float c0 = 4.0f / math::pi;
        float c = c0 / math::Pow(h, 8.0f);
        return (r < h) ? c * math::Pow(h * h - r * r, 3.0f) : 0.0f;
    }
    inline Float2 DPoly6(float r, const Float2& dir, float h)
    {
        constexpr float c0 = 4.0f / math::pi;
        float c = c0 / math::Pow(h, 8.0f);
        return (r < h) ? -6.0f * c * math::Pow(h * h - r * r, 2.0f) * r * dir : Float2::zero;
    }
    inline float DDPoly6(float r, float h)
    {
        constexpr float c0 = 4.0f / math::pi;
        float c = c0 / math::Pow(h, 8.0f);
        float r2 = r * r;
        float h2r2 = h * h - r2;
        return (r < h) ? -12.0f * c * h2r2 * (h * h - 3.0f * r2) : 0.0f;
    }

    // Used for pressure calculations.
    inline float Spiky2(float r, float h)
    {
        constexpr float c0 = 6.0f / math::pi;
        float c = c0 / math::Pow(h, 4.0f);
        return (r < h) ? c * math::Pow(h - r, 2.0f) : 0.0f;
    }
    inline Float2 DSpiky2(float r, const Float2& dir, float h)
    {
        // singular at r = 0.
        constexpr float c0 = 6.0f / math::pi;
        float c = c0 / math::Pow(h, 4.0f);
        return (r < h) ? -2.0f * c * (h - r) * dir : Float2::zero;
    }
    inline float DDSpiky2(float r, float h)
    {
        // singular at r = 0.
        constexpr float c0 = 6.0f / math::pi;
        float c = c0 / math::Pow(h, 4.0f);
        return (r < h) ? 2.0f * c * (2.0f - h / r) : 0.0f;
    }
    inline float Spiky3(float r, float h)
    {
        constexpr float c0 = 10.0f / math::pi;
        float c = c0 / math::Pow(h, 5.0f);
        return (r < h) ? c * math::Pow(h - r, 3.0f) : 0.0f;
    }
    inline Float2 DSpiky3(float r, const Float2& dir, float h)
    {
        // singular at r = 0.
        constexpr float c0 = 10.0f / math::pi;
        float c = c0 / math::Pow(h, 5.0f);
        return (r < h) ? -3.0f * c * math::Pow(h - r, 2.0f) * dir : Float2::zero;
    }
    inline float DDSpiky3(float r, float h)
    {
        // singular at r = 0.
        constexpr float c0 = 10.0f / math::pi;
        float c = c0 / math::Pow(h, 5.0f);
        float hr = h - r;
        return (r < h) ? 3.0f * c * hr * (3.0f - h / r) : 0.0f;
    }
    
    // Used for viscosity calculations:
    inline float Viscos(float r, float h)
    {
        // singular at r = 0.
        constexpr float c0 = 10.0f / (3.0f * math::pi);
        float c = c0 / math::Pow(h, 2.0f);
        return (r < h) ? c * (-0.5f * math::Pow(r / h, 3.0f) + (r * r) / (h  * h) + 0.5f * h / r - 1.0f) : 0.0f;
    }
    inline Float2 DViscos(float r, const Float2& dir, float h)
    {
        // singular at r = 0.
        constexpr float c0 = 10.0f / (3.0f * math::pi);
        float c = c0 / math::Pow(h, 2.0f);
        float r2 = r * r;
        float h2 = h * h;
        return (r < h) ? c * (-1.5f * r2 / (h2 * h) + 2.0f * r / h2 - 0.5f * h / r2) * dir : Float2::zero;
    }
    inline float DDViscos(float r, float h)
    {
        // singular at r = 0.
        constexpr float c0 = 10.0f / (3.0f * math::pi);
        float h2 = h * h;
        float h3 = h2 * h;
        float c = c0 / h2;
        return (r < h) ? c * (4.0f / h2 + 0.5f * h / (r * r * r) - 4.5f * r / h3) : 0.0f;
    }
};