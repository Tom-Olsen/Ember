#include "computePushConstant.hlsli"
#include "math.hlsli"



cbuffer Values : register(b0)
{
    float initialDistributionRadius;
};
RWStructuredBuffer<float2> kp1Buffer : register(u0);
RWStructuredBuffer<float2> kv1Buffer : register(u1);
RWStructuredBuffer<float2> tempPositionBuffer : register(u2);
RWStructuredBuffer<float2> tempVelocityBuffer : register(u3);



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