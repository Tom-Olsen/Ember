#include "computePushConstant.hlsli"
#include "hashGridFunctions2d.hlsli"
#include "math.hlsli"



cbuffer Values : register(b0)
{
    float effectRadius;
    uint hashGridSize; // ~2*particleCount
};
RWStructuredBuffer<uint> cellKeyBuffer : register(u1);
StructuredBuffer<float2> positionBuffer : register(t2);



[numthreads(128, 1, 1)]
void main(uint3 threadID : SV_DispatchThreadID)
{
    uint index = threadID.x;
    if (index < pc.threadCount.x)
    {
        int2 cell = HashGrid2d_Cell(positionBuffer[index], effectRadius);
        uint cellHash = HashGrid2d_CellHash(cell);
        cellKeyBuffer[index] = HashGrid2d_CellKey(cellHash, hashGridSize);
    }
}