#include "computePushConstant.hlsli"
#include "math.hlsli"



cbuffer Values : register(b0)
{
    float initialDistributionRadius;
};
RWStructuredBuffer<float2> positionBuffer : register(u1);
RWStructuredBuffer<float2> velocityBuffer : register(u2);
RWStructuredBuffer<float> densityBuffer : register(u3);
RWStructuredBuffer<float2> forceDensityBuffer : register(u4);
RWStructuredBuffer<float2> kp1Buffer : register(u5);
RWStructuredBuffer<float2> kv1Buffer : register(u6);
RWStructuredBuffer<float2> kp2Buffer : register(u7);
RWStructuredBuffer<float2> kv2Buffer : register(u8);
RWStructuredBuffer<float2> tempPositionBuffer : register(u9);
RWStructuredBuffer<float2> tempVelocityBuffer : register(u10);



[numthreads(128, 1, 1)]
void main(uint3 threadID : SV_DispatchThreadID)
{
    float phi = math_PI * (sqrt(5.0f) - 1.0f);
    
    uint index = threadID.x;
    if (index < pc.threadCount.x)
    {
        float r = index / (pc.threadCount.x - 1.0f) * initialDistributionRadius;
        float theta = phi * index;
        positionBuffer[index] = r * float2(cos(theta), sin(theta));
        velocityBuffer[index] = float2(0, 0);
        densityBuffer[index] = 0.0f;
        forceDensityBuffer[index] = float2(0, 0);
        kp1Buffer[index] = float2(0, 0);
        kv1Buffer[index] = float2(0, 0);
        kp2Buffer[index] = float2(0, 0);
        kv2Buffer[index] = float2(0, 0);
        tempPositionBuffer[index] = float2(0, 0);
        tempVelocityBuffer[index] = float2(0, 0);
    }
}