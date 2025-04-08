#include "computePushConstant.hlsli"



cbuffer Values : register(b0)
{
    float cb_deltaT;
    float cb_maxVelocity;
};
StructuredBuffer<float2> b_forceDensities : register(t1);
StructuredBuffer<float> b_densities : register(t2);
StructuredBuffer<float2> b_kp1 : register(t3);
StructuredBuffer<float2> b_kv1 : register(t4);
StructuredBuffer<float2> b_tempPositions : register(t5);
StructuredBuffer<float2> b_tempVelocities : register(t6);
RWStructuredBuffer<float2> b_kp2 : register(u7);
RWStructuredBuffer<float2> b_kv2 : register(u8);
RWStructuredBuffer<float2> b_positions : register(u9);
RWStructuredBuffer<float2> b_velocities : register(u10);



[numthreads(64, 1, 1)]
void main(uint3 threadID : SV_DispatchThreadID)
{
    const float a1 = 1.0f / 3.0f;
    const float a2 = 2.0f / 3.0f;
    
    uint index = threadID.x;
    if (index < pc.threadCount.x)
    {
        float2 acceleration = b_forceDensities[index] / b_densities[index];
        b_kp2[index] = b_tempVelocities[index];
        b_kv2[index] = acceleration;
        b_positions[index] += (a1 * b_kp1[index] + a2 * b_kp2[index]) * cb_deltaT;
        b_velocities[index] += (a1 * b_kv1[index] + a2 * b_kv2[index]) * cb_deltaT;
        float speed = length(b_velocities[index]);
        if (speed > cb_maxVelocity)
            b_velocities[index] *= (cb_maxVelocity / speed);
    }
}