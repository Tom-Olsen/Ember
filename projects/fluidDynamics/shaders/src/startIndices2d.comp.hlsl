#include "computePushConstant.hlsli"



RWStructuredBuffer<uint> startIndexBuffer : register(u0);
StructuredBuffer<uint> cellKeyBuffer : register(t1);



[numthreads(128, 1, 1)]
void main(uint3 threadID : SV_DispatchThreadID)
{
    uint index = threadID.x;
    if (index < pc.threadCount.x)
    {
        // Second check only if not the first element => no oob access.
        if (index == 0 || cellKeyBuffer[index] != cellKeyBuffer[index - 1])
            startIndexBuffer[cellKeyBuffer[index]] = index;
    }
}