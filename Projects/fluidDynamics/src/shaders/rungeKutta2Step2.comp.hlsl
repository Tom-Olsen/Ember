#include "computePushConstant.hlsli"



cbuffer Values : register(b0)
{
    float deltaT;
    float maxVelocity;
};
StructuredBuffer<float2> forceDensityBuffer : register(t1);
StructuredBuffer<float> densityBuffer : register(t2);
StructuredBuffer<float2> kp1Buffer : register(t3);
StructuredBuffer<float2> kv1Buffer : register(t4);
StructuredBuffer<float2> tempVelocityBuffer : register(t5);
RWStructuredBuffer<float2> kp2Buffer : register(u6);
RWStructuredBuffer<float2> kv2Buffer : register(u7);
RWStructuredBuffer<float2> positionBuffer : register(u8);
RWStructuredBuffer<float2> velocityBuffer : register(u9);



[numthreads(128, 1, 1)]
void main(uint3 threadID : SV_DispatchThreadID)
{
    const float a1 = 1.0f / 3.0f;
    const float a2 = 2.0f / 3.0f;
    
    uint index = threadID.x;
    if (index < pc.threadCount.x)
    {
        float2 acceleration = forceDensityBuffer[index] / densityBuffer[index];
        kp2Buffer[index] = tempVelocityBuffer[index];
        kv2Buffer[index] = acceleration;
        positionBuffer[index] += (a1 * kp1Buffer[index] + a2 * kp2Buffer[index]) * deltaT;
        velocityBuffer[index] += (a1 * kv1Buffer[index] + a2 * kv2Buffer[index]) * deltaT;
        float speed = length(velocityBuffer[index]);
        if (speed > maxVelocity)
            velocityBuffer[index] *= (maxVelocity / speed);
    }
}