#include "computeShaderCommon.hlsli"
#include "hashGridFunctions3d.hlsli"



cbuffer Values : register(b300, SHADER_SET)
{
    float effectRadius;
    uint hashGridSize; // ~2*particleCount
};
RWStructuredBuffer<uint> cellKeyBuffer : register(u200, CALL_SET);
StructuredBuffer<float3> positionBuffer : register(t100, CALL_SET);



[numthreads(128, 1, 1)]
void main(uint3 threadID : SV_DispatchThreadID)
{
    uint index = threadID.x;
    if (index < pc.threadCount.x)
    {
        int3 cell = HashGrid3d_Cell(positionBuffer[index], effectRadius);
        uint cellHash = HashGrid3d_CellHash(cell);
        cellKeyBuffer[index] = HashGrid3d_CellKey(cellHash, hashGridSize);
    }
}