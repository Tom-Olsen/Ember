#pragma once



// Sub libraries:
#include "mathConstants.hlsli"
#include "linearAlgebra.hlsli"
#include "interpolation.hlsli"


// Basic functions:
float Random_FromTime(float time)
{
    return frac(sin(time * 12.9898) * 43758.5453);
}
uint NextPowerOfTwo(uint n)
{
    if (n == 0)
        return 1;
    n--;
    n |= n >> 1;
    n |= n >> 2;
    n |= n >> 4;
    n |= n >> 8;
    n |= n >> 16;
    n++;
    return n;
}