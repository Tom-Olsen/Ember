#include "computeShaderCommon.hlsli"



RWStructuredBuffer<float2> kp1Buffer : register(u200, CALL_SET);
RWStructuredBuffer<float2> kv1Buffer : register(u201, CALL_SET);
RWStructuredBuffer<float2> tempPositionBuffer : register(u202, CALL_SET);
RWStructuredBuffer<float2> tempVelocityBuffer : register(u203, CALL_SET);



[numthreads(128, 1, 1)]
void main(uint3 threadID : SV_DispatchThreadID)
{
    uint index = threadID.x;
    if (index < pc.threadCount.x)
    {
        kp1Buffer[index] = float2(0, 0);
        kv1Buffer[index] = float2(0, 0);
        tempPositionBuffer[index] = float2(0, 0);
        tempVelocityBuffer[index] = float2(0, 0);
    }
}