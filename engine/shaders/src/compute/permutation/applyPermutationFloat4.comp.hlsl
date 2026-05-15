#include "computeShaderCommon.hlsli"
#include "bitonicSortBlockSize.hlsli"



StructuredBuffer<uint> permutationBuffer : register(t100, CALL_SET);
StructuredBuffer<float4> inBuffer : register(t101, CALL_SET);
RWStructuredBuffer<float4> outBuffer : register(u200, CALL_SET);



[numthreads(BLOCK_SIZE, 1, 1)]
void main(uint3 threadID : SV_DispatchThreadID)
{
    uint i = threadID.x;
    if (i < pc.threadCount.x)
        outBuffer[i] = inBuffer[permutationBuffer[i]];
}