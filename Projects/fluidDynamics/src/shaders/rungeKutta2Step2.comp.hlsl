#include "computePushConstant.hlsli"



cbuffer Values : register(b0)
{
    float cb_deltaT;
    float cb_maxVelocity;
};
StructuredBuffer<float2> b_forceDensities : register(t1);
StructuredBuffer<float> b_densities : register(t2);
StructuredBuffer<float2> b_kv1 : register(t3);

RWStructuredBuffer<float2> b_positions : register(u4);
RWStructuredBuffer<float2> b_velocities : register(u5);



[numthreads(64, 1, 1)]
void main(uint3 threadID : SV_DispatchThreadID)
{
    uint index = threadID.x;
    if (index < pc.threadCount.x)
    {
        float2 acceleration = b_forceDensities[index] / b_densities[index];
        b_positions[index] += (b_velocities[index] + 0.5f * b_kv1[index]) * cb_deltaT;
        b_velocities[index] += acceleration * cb_deltaT;
        float speed = length(b_velocities[index]);
        if (speed > cb_maxVelocity)
            b_velocities[index] *= (cb_maxVelocity / speed);
    }
}