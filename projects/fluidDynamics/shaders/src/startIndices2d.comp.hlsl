#include "computePushConstant.hlsli"
#include "math.hlsli"



RWStructuredBuffer<uint> startIndexBuffer : register(u0);
StructuredBuffer<uint> cellKeyBuffer : register(t1);



[numthreads(128, 1, 1)]
void main(uint3 threadID : SV_DispatchThreadID)
{
    int index = int(threadID.x);
    if (index < pc.threadCount.x)
    {
        startIndexBuffer[index] = uint(-1);
        if (index == 0)
            startIndexBuffer[cellKeyBuffer[0]] = 0;
        else
        {
            if (cellKeyBuffer[index] != cellKeyBuffer[index - 1])
                startIndexBuffer[cellKeyBuffer[index]] = index;
        }
    }
}