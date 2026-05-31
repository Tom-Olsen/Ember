#pragma once



// Hash grid primes:
static const uint prime0 = 19349663;    // Not actually prime, but commonly used in spatial hashing.
static const uint prime1 = 73856093;
static const uint prime2 = 83492791;



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



// Hashing:
int3 HashGrid3d_Cell(float3 position, float radius)
{
    return int3(floor(position / radius));
}
uint HashGrid3d_CellHash(int3 cell)
{
    uint hash = (uint(cell.x) * prime0) + (uint(cell.y) * prime1) + (uint(cell.z) * prime2);
    hash ^= hash >> 16;
    hash *= 0x85ebca6b;
    hash ^= hash >> 13;
    hash *= 0xc2b2ae35;
    hash ^= hash >> 16;
    return hash;
}
uint HashGrid3d_CellKey(uint cellHash, uint hashGridSize)
{
    // hashGridSize ~ 2*particleCount for fewer collisions.
    return cellHash % hashGridSize;
}



// Getters:
uint HashGrid3d_GetCellKey(int3 cell, uint hashGridSize)
{
    uint cellHash = HashGrid3d_CellHash(cell);
    return HashGrid3d_CellKey(cellHash, hashGridSize);
}
uint HashGrid3d_GetStartIndex(int3 cell, uint hashGridSize, StructuredBuffer<uint> startIndexBuffer)
{
    uint cellKey = HashGrid3d_GetCellKey(cell, hashGridSize);
    return startIndexBuffer[cellKey];
}