#include "computeShaderCommon.hlsli"
#include "bitonicSortBlockSize.hlsli"



RWStructuredBuffer<uint> indexBuffer : register(u0, SHADER_SET);



[numthreads(BLOCK_SIZE, 1, 1)]
void main(uint3 threadID : SV_DispatchThreadID)
{
    uint i = threadID.x;
    if (i < pc.threadCount.x)
        indexBuffer[i] = i;
}