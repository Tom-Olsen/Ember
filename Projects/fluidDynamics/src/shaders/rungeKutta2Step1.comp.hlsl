#include "computePushConstant.hlsli"



cbuffer Values : register(b0)
{
    float cb_deltaT;
    float cb_maxVelocity;
};
StructuredBuffer<float2> b_forceDensities : register(t1);
StructuredBuffer<float> b_densities : register(t2);
StructuredBuffer<float2> b_positions : register(t3);
StructuredBuffer<float2> b_velocities : register(t4);

RWStructuredBuffer<float2> b_kp1 : register(u5);
RWStructuredBuffer<float2> b_kv1 : register(u6);
RWStructuredBuffer<float2> b_tempPositions : register(u7);
RWStructuredBuffer<float2> b_tempVelocities : register(u8);



[numthreads(64, 1, 1)]
void main(uint3 threadID : SV_DispatchThreadID)
{
    uint index = threadID.x;
    if (index < pc.threadCount.x)
    {
        float2 acceleration = b_forceDensities[index] / b_densities[index];
        b_kp1[index] = b_velocities[index] * cb_deltaT;
        b_kv1[index] = acceleration * cb_deltaT;
        b_tempPositions[index] = b_positions[index] + 0.5f * b_kp1[index];
        b_tempVelocities[index] = b_velocities[index] + 0.5f * b_kv1[index];
        float speed = length(b_tempVelocities[index]);
        if (speed > cb_maxVelocity)
            b_tempVelocities[index] *= (cb_maxVelocity / speed);
    }
}