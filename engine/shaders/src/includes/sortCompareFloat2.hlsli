#ifndef __INCLUDE_GUARD_sortCompareFloat2_hlsli__
#define __INCLUDE_GUARD_sortCompareFloat2_hlsli__



// Sorting algorithms:
bool SortCompareXthenY(float2 a, float2 b)
{
    if (a.x != b.x)
        return a.x > b.x;
    return a.y > b.y;
}
bool SortCompareRadiusThenAngleRadians(float2 a, float2 b)
{
    float radiusA = length(a);
    float radiusB = length(b);
    if (radiusA != radiusB)
        return radiusA > radiusB;

    float aAngleRadians = atan2(a.y, a.x);
    float bAngleRadians = atan2(b.y, b.x);
    if (aAngleRadians < 0)
        aAngleRadians += 2.0f * math_PI;
    if (bAngleRadians < 0)
        bAngleRadians += 2.0f * math_PI;
    return aAngleRadians > bAngleRadians;
}



// Pass through to bitonic sort compute shaders:
bool SortCompare(float2 a, float2 b)
{
    return SortCompareXthenY(a, b);
}



#endif //__INCLUDE_GUARD_sortCompareFloat2_hlsli__