#include "computePushConstant.hlsli"



#define BLOCK_SIZE 128
StructuredBuffer<uint> permutationBuffer : register(t0);
StructuredBuffer<uint> inBuffer : register(t1);
RWStructuredBuffer<uint> outBuffer : register(u2);



[numthreads(BLOCK_SIZE, 1, 1)]
void main(uint3 threadID : SV_DispatchThreadID)
{
    uint i = threadID.x;
    if (i < pc.threadCount.x)
        outBuffer[i] = inBuffer[permutationBuffer[i]];
}