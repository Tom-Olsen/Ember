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
StructuredBuffer<float3> sourcePositionBuffer : register(t102, CALL_SET);
StructuredBuffer<float3> sourceVelocityBuffer : register(t103, CALL_SET);
RWStructuredBuffer<float3> positionBuffer : register(u200, CALL_SET);
RWStructuredBuffer<float3> velocityBuffer : register(u201, CALL_SET);



[numthreads(128, 1, 1)]
void main(uint3 threadID : SV_DispatchThreadID)
{
    uint index = threadID.x;
    if (index < pc.threadCount.x)
    {
        float3 acceleration = forceDensityBuffer[index] / max(densityBuffer[index], 1e-8f);
        float3 velocity = sourceVelocityBuffer[index] + 1.0f * dt * acceleration;
        float speed = length(velocity);
        if (speed > maxVelocity)
            velocity *= maxVelocity / speed;
        positionBuffer[index] = sourcePositionBuffer[index] + 0.5f * dt * velocity;
        velocityBuffer[index] = velocity;
    }
}