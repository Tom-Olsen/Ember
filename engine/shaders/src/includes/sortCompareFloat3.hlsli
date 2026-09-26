#ifndef __INCLUDE_GUARD_sortCompareFloat3_hlsli__
#define __INCLUDE_GUARD_sortCompareFloat3_hlsli__



// Sorting algorithms:
bool SortCompareXthenYthenZ(float3 a, float3 b)
{
    if (a.x != b.x)
        return a.x > b.x;
    if (a.y != b.y)
        return a.y > b.y;
    return a.z > b.z;
}
bool SortCompareRadiusThenAngleRadiansThenAzimuthRadians(float3 a, float3 b)
{
    float radiusA = length(a);
    float radiusB = length(b);
    if (radiusA != radiusB)
        return radiusA > radiusB;

    float aAngleRadians = atan2(length(a.xy), a.z);
    float bAngleRadians = atan2(length(b.xy), b.z);
    if (aAngleRadians != bAngleRadians)
        return aAngleRadians > bAngleRadians;

    float aAzimuthRadians = atan2(a.y, a.x);
    float bAzimuthRadians = atan2(b.y, b.x);
    if (aAzimuthRadians < 0)
        aAzimuthRadians += 2.0f * math_PI;
    if (bAzimuthRadians < 0)
        bAzimuthRadians += 2.0f * math_PI;
    return aAzimuthRadians > bAzimuthRadians;
}



// Pass through to bitonic sort compute shaders:
bool SortCompare(float3 a, float3 b)
{
    return SortCompareXthenYthenZ(a, b);
}



#endif //__INCLUDE_GUARD_sortCompareFloat3_hlsli__