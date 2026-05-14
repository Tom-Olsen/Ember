#ifndef __INCLUDE_GUARD_random_hlsli__
#define __INCLUDE_GUARD_random_hlsli__



// Pseudo random number generators using indexes and time as seeds.
// Range: [0,1)
float Random01(uint index0, uint index1, uint index2, uint index3, float time)
{
    // Sorting network:
    if(index0>index1){uint t=index0; index0=index1; index1=t;}
    if(index2>index3){uint t=index2; index2=index3; index3=t;}
    if(index0>index2){uint t=index0; index0=index2; index2=t;}
    if(index1>index3){uint t=index1; index1=index3; index3=t;}
    if(index1>index2){uint t=index1; index1=index2; index2=t;}

    // Generate seed:
    uint timeSeed = asuint(time * 1000.0f);
    uint seed =
        index0 * 73856093u ^
        index1 * 83492791u ^
        index2 * 19349663u ^
        index3 * 2971215073u ^
        timeSeed * 39916801u;

    // Final avalanche:
    seed ^= seed >> 16;
    seed *= 0x85ebca6bu;
    seed ^= seed >> 13;
    seed *= 0xc2b2ae35u;
    seed ^= seed >> 16;
    return float(seed) * (1.0f / 4294967296.0f); // [0,1)
}
float Random01(uint index0, uint index1, uint index2, float time)
{
    return Random01(index0, index1, index2, 0u, time);
}
float Random01(uint index0, uint index1, float time)
{
    return Random01(index0, index1, 0u, 0u, time);
}
float Random01(uint index0, float time)
{
    return Random01(index0, 0u, 0u, 0u, time);
}
float Random01(float time)
{
    return Random01(0u, 0u, 0u, 0u, time);
}



#endif // __INCLUDE_GUARD_random_hlsli__