#ifndef __INCLUDE_GUARD_interpolation_hlsli__
#define __INCLUDE_GUARD_interpolation_hlsli__
#include "mathConstants.hlsli"



// Linear Interpolation, x€[0,1], f0=f(x=0), f1=f(x=1).
float Interpolation_Linear(float x, float f0, float f1)
{
    return f0 * (1.0 - x) + f1 * x;
}

// Cubic Interpolation, x€[0,1], f0=f(x=0), f1=f(x=1), f2=f(x=2).
float Interpolation_Cubic(float x, float fm1, float fp0, float fp1, float fp2)
{
    float c0 = fp0;
    float c1 = 0.5 * (fp1 - fm1);
    float c2 = fm1 - 2.5 * fp0 + 2.0 * fp1 - 0.5 * fp2;
    float c3 = 0.5 * (fp2 - fm1) + 1.5 * (fp0 - fp1);
    return ((c3 * x + c2) * x + c1) * x + c0;
}

// Bilinear Interpolation, x,y€[0,1]
// Calculates f(x,y) from f(0,0), f(0,1), f(1,0) and f(1,1):
// f(0,1) --- f(1,1)
//    |  (x,y)   |
// f(0,0) --- f(1,0)
// f10 <=> f(1,0)
float Interpolation_Bilinear(float2 p, float f00, float f01, float f10, float f11)
{
    float f0 = Interpolation_Linear(p.x, f00, f10);
    float f1 = Interpolation_Linear(p.x, f01, f11);
    return f0 * (1.0 - p.y) + f1 * p.y;
}

// Bicubic Interpolation, x€[0,1],
// fm1m1=f(-1,-1), fm1p0=f(-1,0), fm1p1=f(-1,1), fm1p2=f(-1,2).
// fp0m1=f( 0,-1), fp0p0=f( 0,0), fp0p1=f( 0,1), fp0p2=f( 0,2).
// fp1m1=f( 1,-1), fp1p0=f( 1,0), fp1p1=f( 1,1), fp1p2=f( 1,2).
// fp2m1=f( 2,-1), fp2p0=f( 2,0), fp2p1=f( 2,1), fp2p2=f( 2,2).
float Interpolation_Bicubic(float2 p,
                            float fm1m1, float fm1p0, float fm1p1, float fm1p2,
                            float fp0m1, float fp0p0, float fp0p1, float fp0p2,
                            float fp1m1, float fp1p0, float fp1p1, float fp1p2,
                            float fp2m1, float fp2p0, float fp2p1, float fp2p2)
{
    float Fm1 = Interpolation_Cubic(p.x, fm1m1, fp0m1, fp1m1, fp2m1);
    float Fp0 = Interpolation_Cubic(p.x, fm1p0, fp0p0, fp1p0, fp2p0);
    float Fp1 = Interpolation_Cubic(p.x, fm1p1, fp0p1, fp1p1, fp2p1);
    float Fp2 = Interpolation_Cubic(p.x, fm1p2, fp0p2, fp1p2, fp2p2);
    return Interpolation_Cubic(p.y, Fm1, Fp0, Fp1, Fp2);
}

#endif //__INCLUDE_GUARD_interpolation_hlsli__