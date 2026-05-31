#ifndef __INCLUDE_GUARD_sortCompareFloat2_hlsli__
#define __INCLUDE_GUARD_sortCompareFloat2_hlsli__



// Sorting algorithms:
bool SortCompareXthenY(float2 a, float2 b)
{
    if (a.x != b.x)
        return a.x > b.x;
    return a.y > b.y;
}
bool SortCompareRadiusThenAngle(float2 a, float2 b)
{
    float radiusA = length(a);
    float radiusB = length(b);
    if (radiusA != radiusB)
        return radiusA > radiusB;

    float angleA = atan2(a.y, a.x);
    float angleB = atan2(b.y, b.x);
    if (angleA < 0)
        angleA += 2.0f * math_PI;
    if (angleB < 0)
        angleB += 2.0f * math_PI;
    return angleA > angleB;
}



// Pass through to bitonic sort compute shaders:
bool SortCompare(float2 a, float2 b)
{
    return SortCompareXthenY(a, b);
}



#endif //__INCLUDE_GUARD_sortCompareFloat2_hlsli__