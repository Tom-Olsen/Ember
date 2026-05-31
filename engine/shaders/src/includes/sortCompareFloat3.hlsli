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
bool SortCompareRadiusThenAngleThenAzimuth(float3 a, float3 b)
{
    float radiusA = length(a);
    float radiusB = length(b);
    if (radiusA != radiusB)
        return radiusA > radiusB;

    float angleA = atan2(length(a.xy), a.z);
    float angleB = atan2(length(b.xy), b.z);
    if (angleA != angleB)
        return angleA > angleB;

    float azimuthA = atan2(a.y, a.x);
    float azimuthB = atan2(b.y, b.x);
    if (azimuthA < 0)
        azimuthA += 2.0f * math_PI;
    if (azimuthB < 0)
        azimuthB += 2.0f * math_PI;
    return azimuthA > azimuthB;
}



// Pass through to bitonic sort compute shaders:
bool SortCompare(float3 a, float3 b)
{
    return SortCompareXthenYthenZ(a, b);
}



#endif //__INCLUDE_GUARD_sortCompareFloat3_hlsli__