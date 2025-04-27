#ifndef __INCLUDE_GUARD_hashGridFunctions3d_hlsli__
#define __INCLUDE_GUARD_hashGridFunctions3d_hlsli__



// Hash grid primes:
static const int prime0 = 15731;
static const int prime1 = 789221;
static const int prime2 = 1376312589;



// Offsets for neighbour grid cells:
static const int3 offsets[27] =
{
    int3(-1, -1, -1), int3(-1, -1, 0), int3(-1, -1, 1),
    int3(-1,  0, -1), int3(-1,  0, 0), int3(-1,  0, 1),
    int3(-1,  1, -1), int3(-1,  1, 0), int3(-1,  1, 1),
    int3( 0, -1, -1), int3( 0, -1, 0), int3( 0, -1, 1),
    int3( 0,  0, -1), int3( 0,  0, 0), int3( 0,  0, 1),
    int3( 0,  1, -1), int3( 0,  1, 0), int3( 0,  1, 1),
    int3( 1, -1, -1), int3( 1, -1, 0), int3( 1, -1, 1),
    int3( 1,  0, -1), int3( 1,  0, 0), int3( 1,  0, 1),
    int3( 1,  1, -1), int3( 1,  1, 0), int3( 1,  1, 1)
};



int3 Cell(float3 position, float radius)
{
    return int3(floor(position / radius));
}
int CellHash(int3 cell)
{
    int a = cell.x * prime0;
    int b = cell.y * prime1;
    int c = cell.z * prime2;
    return a + b + c;
}
uint CellKey(int cellHash, int particleCount)
{
    // Module operation with negative numbers is not identical between c++ and hlsl.
    // The below method circumvents this discrepency by using the abs value.
    if (cellHash < 0)
        return particleCount - (abs(cellHash) % particleCount);
    else
        return cellHash % particleCount;
}



#endif //__INCLUDE_GUARD_hashGridFunctions3d_hlsli__