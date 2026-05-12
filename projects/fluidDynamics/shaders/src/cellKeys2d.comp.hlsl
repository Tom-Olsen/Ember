#include "computeShaderCommon.hlsli"
#include "hashGridFunctions2d.hlsli"



cbuffer Values : register(b300, SHADER_SET)
{
    float effectRadius;
    uint hashGridSize; // ~2*particleCount
};
RWStructuredBuffer<uint> cellKeyBuffer : register(u200, SHADER_SET);
StructuredBuffer<float2> positionBuffer : register(t100, SHADER_SET);



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