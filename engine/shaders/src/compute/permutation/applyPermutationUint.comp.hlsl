#include "computePushConstant.hlsli"



#define BLOCK_SIZE 128
StructuredBuffer<uint> permutationBuffer : register(t0, SHADER_SET);
StructuredBuffer<uint> inBuffer : register(t1, SHADER_SET);
RWStructuredBuffer<uint> outBuffer : register(u2, SHADER_SET);



[numthreads(BLOCK_SIZE, 1, 1)]
void main(uint3 threadID : SV_DispatchThreadID)
{
    uint i = threadID.x;
    if (i < pc.threadCount.x)
        outBuffer[i] = inBuffer[permutationBuffer[i]];
}