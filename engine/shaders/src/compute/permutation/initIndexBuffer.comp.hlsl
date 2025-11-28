#include "computePushConstant.hlsli"



#define BLOCK_SIZE 128
RWStructuredBuffer<uint> indexBuffer : register(u0);



[numthreads(BLOCK_SIZE, 1, 1)]
void main(uint3 threadID : SV_DispatchThreadID)
{
    uint i = threadID.x;
    if (i < pc.threadCount.x)
        indexBuffer[i] = i;
}