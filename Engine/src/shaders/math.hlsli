#ifndef __INCLUDE_GUARD_mathf_hlsli__
#define __INCLUDE_GUARD_mathf_hlsli__



// Sub libraries:
#include "mathConstants.hlsli"
#include "linearAlgebra.hlsli"
#include "interpolation.hlsli"


// Basic functions:
float Random_FromTime(float time)
{
    return frac(sin(time * 12.9898) * 43758.5453);
}



#endif // __INCLUDE_GUARD_mathf_hlsli__