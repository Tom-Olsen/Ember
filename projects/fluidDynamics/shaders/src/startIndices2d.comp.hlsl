#include "computeShaderCommon.hlsli"



RWStructuredBuffer<uint> startIndexBuffer : register(u200, SHADER_SET);
StructuredBuffer<uint> cellKeyBuffer : register(t100, SHADER_SET);



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