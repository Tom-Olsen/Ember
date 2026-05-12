#include "computeShaderCommon.hlsli"



RWStructuredBuffer<uint> startIndexBuffer : register(u200, SHADER_SET);
StructuredBuffer<uint> cellKeyBuffer : register(t100, SHADER_SET);



[numthreads(128, 1, 1)]
void main(uint3 threadID : SV_DispatchThreadID)
{
    int index = int(threadID.x);
    if (index < pc.threadCount.x)
    {
        startIndexBuffer[index] = -1;
        if (index == 0)
            startIndexBuffer[cellKeyBuffer[0]] = 0;
        else
        {
            if (cellKeyBuffer[index] != cellKeyBuffer[index - 1])
                startIndexBuffer[cellKeyBuffer[index]] = index;
        }
    }
}