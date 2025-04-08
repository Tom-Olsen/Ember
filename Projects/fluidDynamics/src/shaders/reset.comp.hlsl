#include "computePushConstant.hlsli"
#include "math.hlsli"



cbuffer Values : register(b0)
{
    float cb_initialDistributionRadius;
};
RWStructuredBuffer<float2> b_positions : register(u1);
RWStructuredBuffer<float2> b_velocities : register(u2);
RWStructuredBuffer<float>  b_densities : register(u3);
RWStructuredBuffer<float2> b_forceDensities : register(u4);
RWStructuredBuffer<float2> b_kp1 : register(u5);
RWStructuredBuffer<float2> b_kv1 : register(u6);
RWStructuredBuffer<float2> b_kp2 : register(u7);
RWStructuredBuffer<float2> b_kv2 : register(u8);
RWStructuredBuffer<float2> b_tempPositions : register(u9);
RWStructuredBuffer<float2> b_tempVelocities : register(u10);



[numthreads(64, 1, 1)]
void main(uint3 threadID : SV_DispatchThreadID)
{
    float phi = math_PI * (sqrt(5.0f) - 1.0f);
    
    uint index = threadID.x;
    if (index < pc.threadCount.x)
    {
        float r = index / (pc.threadCount.x - 1.0f) * cb_initialDistributionRadius;
        float theta = phi * index;
        b_positions[index] = r * float2(cos(theta), sin(theta));
        b_velocities[index] = float2(0, 0);
        b_densities[index] = 0.0f;
        b_forceDensities[index] = float2(0, 0);
        b_kp1[index] = float2(0, 0);
        b_kv1[index] = float2(0, 0);
        b_kp2[index] = float2(0, 0);
        b_kv2[index] = float2(0, 0);
        b_tempPositions[index] = float2(0, 0);
        b_tempVelocities[index] = float2(0, 0);
    }
}