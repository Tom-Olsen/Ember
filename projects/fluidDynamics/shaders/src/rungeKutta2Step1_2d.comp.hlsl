#include "computePushConstant.hlsli"



cbuffer Values : register(b0)
{
    float deltaT;
    float maxVelocity;
};
StructuredBuffer<float2> forceDensityBuffer : register(t1);
StructuredBuffer<float> densityBuffer : register(t2);
StructuredBuffer<float2> positionBuffer : register(t3);
StructuredBuffer<float2> velocityBuffer : register(t4);
RWStructuredBuffer<float2> kp1Buffer : register(u5);
RWStructuredBuffer<float2> kv1Buffer : register(u6);
RWStructuredBuffer<float2> tempPositionBuffer : register(u7);
RWStructuredBuffer<float2> tempVelocityBuffer : register(u8);



[numthreads(128, 1, 1)]
void main(uint3 threadID : SV_DispatchThreadID)
{
    const float q  = 3.0f / 4.0f;
    
    uint index = threadID.x;
    if (index < pc.threadCount.x)
    {
        float2 acceleration = forceDensityBuffer[index] / densityBuffer[index];
        kp1Buffer[index] = velocityBuffer[index];
        kv1Buffer[index] = acceleration;
        tempPositionBuffer[index] = positionBuffer[index] + q * kp1Buffer[index] * deltaT;
        tempVelocityBuffer[index] = velocityBuffer[index] + q * kv1Buffer[index] * deltaT;
        float speed = length(tempVelocityBuffer[index]);
        if (speed > maxVelocity)
            tempVelocityBuffer[index] *= (maxVelocity / speed);
    }
}