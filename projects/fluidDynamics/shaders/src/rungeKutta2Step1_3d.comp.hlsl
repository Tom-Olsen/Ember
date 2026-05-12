#include "computeShaderCommon.hlsli"



cbuffer Values : register(b300, SHADER_SET)
{
    float deltaT;
    float maxVelocity;
};
StructuredBuffer<float3> forceDensityBuffer : register(t100, SHADER_SET);
StructuredBuffer<float> densityBuffer : register(t101, SHADER_SET);
StructuredBuffer<float3> positionBuffer : register(t102, SHADER_SET);
StructuredBuffer<float3> velocityBuffer : register(t103, SHADER_SET);
RWStructuredBuffer<float3> kp1Buffer : register(u200, SHADER_SET);
RWStructuredBuffer<float3> kv1Buffer : register(u201, SHADER_SET);
RWStructuredBuffer<float3> tempPositionBuffer : register(u202, SHADER_SET);
RWStructuredBuffer<float3> tempVelocityBuffer : register(u203, SHADER_SET);



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