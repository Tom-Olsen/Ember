#include "computePushConstant.hlsli"



RWStructuredBuffer<uint> startIndexBuffer : register(u0);



[numthreads(128, 1, 1)]
void main(uint3 threadID : SV_DispatchThreadID)
{
    uint index = threadID.x;
    if (index < pc.threadCount.x)
        startIndexBuffer[index] = uint(-1);
}