#include "computePushConstant.hlsli"
#include "math.hlsli"



cbuffer Values : register(b0)
{
    float initialDistributionRadius;
};
RWStructuredBuffer<float3> positionBuffer : register(u1);
RWStructuredBuffer<float3> velocityBuffer : register(u2);
RWStructuredBuffer<float> densityBuffer : register(u3);
RWStructuredBuffer<float3> forceDensityBuffer : register(u4);
RWStructuredBuffer<float3> kp1Buffer : register(u5);
RWStructuredBuffer<float3> kv1Buffer : register(u6);
RWStructuredBuffer<float3> kp2Buffer : register(u7);
RWStructuredBuffer<float3> kv2Buffer : register(u8);
RWStructuredBuffer<float3> tempPositionBuffer : register(u9);
RWStructuredBuffer<float3> tempVelocityBuffer : register(u10);



[numthreads(128, 1, 1)]
void main(uint3 threadID : SV_DispatchThreadID)
{
    float phi = math_PI * (sqrt(5.0f) - 1.0f);
    
    uint index = threadID.x;
    if (index < pc.threadCount.x)
    {
        float r = index / (pc.threadCount.x - 1.0f) * initialDistributionRadius;
        float theta = phi * index;
        positionBuffer[index] = r * float3(cos(theta), sin(theta), 0); // TODO: use fibonacci sphere
        velocityBuffer[index] = float3(0, 0, 0);
        densityBuffer[index] = 0.0f;
        forceDensityBuffer[index] = float3(0, 0, 0);
        kp1Buffer[index] = float3(0, 0, 0);
        kv1Buffer[index] = float3(0, 0, 0);
        kp2Buffer[index] = float3(0, 0, 0);
        kv2Buffer[index] = float3(0, 0, 0);
        tempPositionBuffer[index] = float3(0, 0, 0);
        tempVelocityBuffer[index] = float3(0, 0, 0);
    }
}