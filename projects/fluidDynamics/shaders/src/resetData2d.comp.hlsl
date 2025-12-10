#include "computePushConstant.hlsli"
#include "math.hlsli"



cbuffer Values : register(b0)
{
    uint hashGridSize; // 2 * particleCount rounded up to next prime.
    float initialDistributionRadius;
};
RWStructuredBuffer<uint> cellKeyBuffer : register(u1);
RWStructuredBuffer<uint> startIndexBuffer : register(u2);
RWStructuredBuffer<uint> sortPermutationBuffer : register(u3);
RWStructuredBuffer<float2> positionBuffer : register(u4);
RWStructuredBuffer<float2> velocityBuffer : register(u5);
RWStructuredBuffer<float> densityBuffer : register(u6);
RWStructuredBuffer<float2> normalBuffer : register(u7);
RWStructuredBuffer<float> curvatureBuffer : register(u8);
RWStructuredBuffer<float2> forceDensityBuffer : register(u9);
RWStructuredBuffer<float2> tempBuffer0 : register(u10);
RWStructuredBuffer<float2> tempBuffer1 : register(u11);
RWStructuredBuffer<float2> tempBuffer2 : register(u12);
RWStructuredBuffer<float2> tempBuffer3 : register(u13);
RWStructuredBuffer<float2> tempBuffer4 : register(u14);
RWStructuredBuffer<float2> tempBuffer5 : register(u15);



[numthreads(128, 1, 1)]
void main(uint3 threadID : SV_DispatchThreadID)
{
    float phi = math_PI * (sqrt(5.0f) - 1.0f);
    
    uint index = threadID.x;
    if (index < pc.threadCount.x)
    {
        cellKeyBuffer[index] = -1;
        startIndexBuffer[index] = -1;
        startIndexBuffer[index + pc.threadCount.x] = -1;
        if (index + 2 * pc.threadCount.x < hashGridSize) // startIndexBuffer has size hashGridSize.
            startIndexBuffer[index + 2 * pc.threadCount.x] = -1;
        sortPermutationBuffer[index] = -1;
        
        float r = index / (pc.threadCount.x - 1.0f) * initialDistributionRadius;
        float theta = phi * index;
        positionBuffer[index] = r * float2(cos(theta), sin(theta));
        
        velocityBuffer[index] = float2(0, 0);
        densityBuffer[index] = 0.0f;
        normalBuffer[index] = float2(0, 0);
        curvatureBuffer[index] = 0;
        forceDensityBuffer[index] = float2(0, 0);
        tempBuffer0[index] = float2(0, 0);
        tempBuffer1[index] = float2(0, 0);
        tempBuffer2[index] = float2(0, 0);
        tempBuffer3[index] = float2(0, 0);
        tempBuffer4[index] = float2(0, 0);
        tempBuffer5[index] = float2(0, 0);
    }
}