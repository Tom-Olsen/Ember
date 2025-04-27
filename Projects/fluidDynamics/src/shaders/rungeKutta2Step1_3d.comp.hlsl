#include "computePushConstant.hlsli"



cbuffer Values : register(b0)
{
    float deltaT;
    float maxVelocity;
};
StructuredBuffer<float3> forceDensityBuffer : register(t1);
StructuredBuffer<float> densityBuffer : register(t2);
StructuredBuffer<float3> positionBuffer : register(t3);
StructuredBuffer<float3> velocityBuffer : register(t4);
RWStructuredBuffer<float3> kp1Buffer : register(u5);
RWStructuredBuffer<float3> kv1Buffer : register(u6);
RWStructuredBuffer<float3> tempPositionBuffer : register(u7);
RWStructuredBuffer<float3> tempVelocityBuffer : register(u8);



[numthreads(128, 1, 1)]
void main(uint3 threadID : SV_DispatchThreadID)
{
    const float q  = 3.0f / 4.0f;
    
    uint index = threadID.x;
    if (index < pc.threadCount.x)
    {
        float3 acceleration = forceDensityBuffer[index] / densityBuffer[index];
        kp1Buffer[index] = velocityBuffer[index];
        kv1Buffer[index] = acceleration;
        tempPositionBuffer[index] = positionBuffer[index] + q * kp1Buffer[index] * deltaT;
        tempVelocityBuffer[index] = velocityBuffer[index] + q * kv1Buffer[index] * deltaT;
        float speed = length(tempVelocityBuffer[index]);
        if (speed > maxVelocity)
            tempVelocityBuffer[index] *= (maxVelocity / speed);
    }
}