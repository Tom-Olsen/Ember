#include "computePushConstant.hlsli"
#include "hashGridFunctions3d.hlsli"
#include "math.hlsli"



cbuffer Values : register(b0)
{
    float gridRadius;
    int particleCount;
};
RWStructuredBuffer<uint> cellKeyBuffer : register(u1);
StructuredBuffer<float3> positionBuffer : register(t2);



[numthreads(128, 1, 1)]
void main(uint3 threadID : SV_DispatchThreadID)
{
    int index = int(threadID.x);
    if (index < pc.threadCount.x)
    {
        int3 cell = Cell(positionBuffer[index], gridRadius);
        int cellHash = CellHash(cell);
        cellKeyBuffer[index] = CellKey(cellHash, particleCount);
    }
}