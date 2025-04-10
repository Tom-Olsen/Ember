#include "computePushConstant.hlsli"



RWStructuredBuffer<int> b_data : register(u0);



[numthreads(32, 1, 1)]
void main(uint3 threadID : SV_DispatchThreadID)
{
    uint index = threadID.x;
    if (index < pc.threadCount.x)
    {
        b_data[index] = int(index);
    }
}