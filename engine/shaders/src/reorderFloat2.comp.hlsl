#include "computePushConstant.hlsli"



#define BLOCK_SIZE 128
StructuredBuffer<uint> indexBuffer : register(t0);
StructuredBuffer<float2> inBuffer : register(t1);
RWStructuredBuffer<float2> outBuffer : register(u0);



[numthreads(BLOCK_SIZE, 1, 1)]
void main(uint3 threadID : SV_DispatchThreadID)
{
    uint i = threadID.x;
    if (i < pc.threadCount.x)
        outBuffer[i] = inBuffer[indexBuffer[i]];
}