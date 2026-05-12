#include "computeShaderCommon.hlsli"



cbuffer Values : register(b300, SHADER_SET)
{
    float dt;
    float maxVelocity;
};
StructuredBuffer<float2> forceDensityBuffer : register(t100, SHADER_SET);
StructuredBuffer<float> densityBuffer : register(t101, SHADER_SET);
StructuredBuffer<float2> kp1Buffer : register(t102, SHADER_SET);
StructuredBuffer<float2> kv1Buffer : register(t103, SHADER_SET);
StructuredBuffer<float2> tempVelocityBuffer : register(t104, SHADER_SET);
RWStructuredBuffer<float2> positionBuffer : register(u200, SHADER_SET);
RWStructuredBuffer<float2> velocityBuffer : register(u201, SHADER_SET);



[numthreads(128, 1, 1)]
void main(uint3 threadID : SV_DispatchThreadID)
{
    const float a1 = 1.0f / 3.0f;
    const float a2 = 2.0f / 3.0f;
    
    uint index = threadID.x;
    if (index < pc.threadCount.x)
    {
        float2 acceleration = forceDensityBuffer[index] / densityBuffer[index];
        float2 kp2 = tempVelocityBuffer[index];
        float2 kv2 = acceleration;
        positionBuffer[index] += (a1 * kp1Buffer[index] + a2 * kp2) * dt;
        velocityBuffer[index] += (a1 * kv1Buffer[index] + a2 * kv2) * dt;
        float speed = length(velocityBuffer[index]);
        if (speed > maxVelocity)
            velocityBuffer[index] *= (maxVelocity / speed);
    }
}