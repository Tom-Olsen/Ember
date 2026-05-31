#include "computeShaderCommon.hlsli"



cbuffer Values : register(b300, SHADER_SET)
{
    float dt;
    float maxVelocity;
};
StructuredBuffer<float3> forceDensityBuffer : register(t100, CALL_SET);
StructuredBuffer<float> densityBuffer : register(t101, CALL_SET);
StructuredBuffer<float3> kp1Buffer : register(t102, CALL_SET);
StructuredBuffer<float3> kv1Buffer : register(t103, CALL_SET);
StructuredBuffer<float3> tempVelocityBuffer : register(t104, CALL_SET);
RWStructuredBuffer<float3> positionBuffer : register(u200, CALL_SET);
RWStructuredBuffer<float3> velocityBuffer : register(u201, CALL_SET);



[numthreads(128, 1, 1)]
void main(uint3 threadID : SV_DispatchThreadID)
{
    const float a1 = 1.0f / 3.0f;
    const float a2 = 2.0f / 3.0f;

    uint index = threadID.x;
    if (index < pc.threadCount.x)
    {
        float3 acceleration = forceDensityBuffer[index] / max(densityBuffer[index], 1e-8f);
        float3 kp2 = tempVelocityBuffer[index];
        float3 kv2 = acceleration;
        positionBuffer[index] += (a1 * kp1Buffer[index] + a2 * kp2) * dt;
        velocityBuffer[index] += (a1 * kv1Buffer[index] + a2 * kv2) * dt;
        float speed = length(velocityBuffer[index]);
        if (speed > maxVelocity)
            velocityBuffer[index] *= (maxVelocity / speed);
    }
}