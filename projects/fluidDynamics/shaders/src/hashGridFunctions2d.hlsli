#pragma once



// Hash grid primes:
static const uint prime0 = 73856093;
static const uint prime1 = 83492791;



// Offsets for neighbour grid cells:
static const int2 offsets[9] =
{
    int2(-1, -1), int2(-1, 0), int2(-1, 1),
    int2( 0, -1), int2( 0, 0), int2( 0, 1),
    int2( 1, -1), int2( 1, 0), int2( 1, 1)
};



// Hashing:
int2 HashGrid2d_Cell(float2 position, float radius)
{
    return int2(floor(position / radius));
}
uint HashGrid2d_CellHash(int2 cell)
{
    uint hash = (uint(cell.x) * prime0) + (uint(cell.y) * prime1);
    hash ^= hash >> 16;
    hash *= 0x85ebca6b;
    hash ^= hash >> 13;
    hash *= 0xc2b2ae35;
    hash ^= hash >> 16;
    return hash;
}
uint HashGrid2d_CellKey(int cellHash, int hashGridSize)
{
    // hashGridSize ~ 2*particleCount for fever collisions.
    return cellHash % hashGridSize;
}



// Getters:
uint HashGrid2d_GetCellKey(int2 cell, int hashGridSize)
{
    uint cellHash = HashGrid2d_CellHash(cell);
    return HashGrid2d_CellKey(cellHash, hashGridSize);
}
uint HashGrid2d_GetStartIndex(int2 cell, int hashGridSize, StructuredBuffer<uint> startIndexBuffer)
{
    uint cellKey = HashGrid2d_GetCellKey(cell, hashGridSize);
    return startIndexBuffer[cellKey];
}