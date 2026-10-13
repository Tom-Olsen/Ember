#include "computeShaderCommon.hlsli"



cbuffer ShaderValues : register(b300, SHADER_SET)
{
    float maxVelocity;
};
cbuffer CallValues : register(b301, CALL_SET)
{
    float dt;
};
StructuredBuffer<float3> forceDensityBuffer : register(t100, CALL_SET);
StructuredBuffer<float> densityBuffer : register(t101, CALL_SET);
StructuredBuffer<float3> positionBuffer : register(t102, CALL_SET);
StructuredBuffer<float3> velocityBuffer : register(t103, CALL_SET);
RWStructuredBuffer<float3> kp1Buffer : register(u200, CALL_SET);
RWStructuredBuffer<float3> kv1Buffer : register(u201, CALL_SET);
RWStructuredBuffer<float3> tempPositionBuffer : register(u202, CALL_SET);
RWStructuredBuffer<float3> tempVelocityBuffer : register(u203, CALL_SET);



[numthreads(128, 1, 1)]
void main(uint3 threadID : SV_DispatchThreadID)
{
    const float q  = 3.0f / 4.0f;

    uint index = threadID.x;
    if (index < pc.threadCount.x)
    {
        float3 acceleration = forceDensityBuffer[index] / max(densityBuffer[index], 1e-8f);
        float3 velocity = velocityBuffer[index];
        float3 tempPosition = positionBuffer[index] + q * velocity * dt;
        float3 tempVelocity = velocity + q * acceleration * dt;
        float speed = length(tempVelocity);
        if (speed > maxVelocity)
            tempVelocity *= (maxVelocity / speed);
        kp1Buffer[index] = velocity;
        kv1Buffer[index] = acceleration;
        tempPositionBuffer[index] = tempPosition;
        tempVelocityBuffer[index] = tempVelocity;
    }
}