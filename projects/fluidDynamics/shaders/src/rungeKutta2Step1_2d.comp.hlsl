#include "computeShaderCommon.hlsli"



cbuffer Values : register(b300, SHADER_SET)
{
    float dt;
    float maxVelocity;
};
StructuredBuffer<float2> forceDensityBuffer : register(t100, CALL_SET);
StructuredBuffer<float> densityBuffer : register(t101, CALL_SET);
StructuredBuffer<float2> positionBuffer : register(t102, CALL_SET);
StructuredBuffer<float2> velocityBuffer : register(t103, CALL_SET);
RWStructuredBuffer<float2> kp1Buffer : register(u200, CALL_SET);
RWStructuredBuffer<float2> kv1Buffer : register(u201, CALL_SET);
RWStructuredBuffer<float2> tempPositionBuffer : register(u202, CALL_SET);
RWStructuredBuffer<float2> tempVelocityBuffer : register(u203, CALL_SET);



[numthreads(128, 1, 1)]
void main(uint3 threadID : SV_DispatchThreadID)
{
    const float q  = 3.0f / 4.0f;

    uint index = threadID.x;
    if (index < pc.threadCount.x)
    {
        float2 acceleration = forceDensityBuffer[index] / max(densityBuffer[index], 1e-8f);
        kp1Buffer[index] = velocityBuffer[index];
        kv1Buffer[index] = acceleration;
        tempPositionBuffer[index] = positionBuffer[index] + q * kp1Buffer[index] * dt;
        tempVelocityBuffer[index] = velocityBuffer[index] + q * kv1Buffer[index] * dt;
        float speed = length(tempVelocityBuffer[index]);
        if (speed > maxVelocity)
            tempVelocityBuffer[index] *= (maxVelocity / speed);
    }
}