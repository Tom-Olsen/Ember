#ifndef __INCLUDE_GUARD_hashGridFunctions2d_hlsli__
#define __INCLUDE_GUARD_hashGridFunctions2d_hlsli__



// Hash grid primes:
static const int prime0 = 73856093;
static const int prime1 = 83492791;



// Offsets for neighbour grid cells:
static const int2 offsets[9] =
{
    int2(-1, -1), int2(-1, 0), int2(-1, 1),
    int2( 0, -1), int2( 0, 0), int2( 0, 1),
    int2( 1, -1), int2( 1, 0), int2( 1, 1)
};



int2 Cell(float2 position, float radius)
{
    return int2(floor(position / radius));
}
int CellHash(int2 cell)
{
    int a = cell.x * prime0;
    int b = cell.y * prime1;
    return a ^ b;
}
uint CellKey(int cellHash, int particleCount)
{
    // Module operation with negative numbers is not identical between c++ and hlsl.
    // The below method circumvents this discrepency.
    int mod = cellHash % particleCount;
    return (mod + particleCount) % particleCount;
}



#endif //__INCLUDE_GUARD_hashGridFunctions2d_hlsli__