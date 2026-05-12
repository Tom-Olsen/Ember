#include "computeShaderCommon.hlsli"
#include "hashGridFunctions3d.hlsli"



cbuffer Values : register(b300, SHADER_SET)
{
    float gridRadius;
    uint particleCount;
};
RWStructuredBuffer<uint> cellKeyBuffer : register(u200, SHADER_SET);
StructuredBuffer<float3> positionBuffer : register(t100, SHADER_SET);



[numthreads(128, 1, 1)]
void main(uint3 threadID : SV_DispatchThreadID)
{
    uint index = threadID.x;
    if (index < pc.threadCount.x)
    {
        int3 cell = Cell(positionBuffer[index], gridRadius);
        uint cellHash = CellHash(cell);
        cellKeyBuffer[index] = CellKey(cellHash, particleCount);
    }
}