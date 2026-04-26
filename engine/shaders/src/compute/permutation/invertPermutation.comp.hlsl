#include "computePushConstant.hlsli"
#include "descriptorSetMacros.h"



#define BLOCK_SIZE 128
StructuredBuffer<uint> permutationBuffer : register(t0, SHADER_SET);
RWStructuredBuffer<uint> inversePermutationBuffer : register(u1, SHADER_SET);



[numthreads(BLOCK_SIZE, 1, 1)]
void main(uint3 threadID : SV_DispatchThreadID)
{
    uint i = threadID.x;
    if (i < pc.threadCount.x)
        inversePermutationBuffer[permutationBuffer[i]] = i;
}