#include "computeShaderCommon.hlsli"



cbuffer CallValues : register(b300, CALL_SET)
{
    uint hashGridSize; // 2 * particleCount rounded up to next prime.
    float initialDistributionRadius;
};
RWStructuredBuffer<uint> cellKeyBuffer : register(u200, CALL_SET);
RWStructuredBuffer<uint> startIndexBuffer : register(u201, CALL_SET);
RWStructuredBuffer<uint> sortPermutationBuffer : register(u202, CALL_SET);
RWStructuredBuffer<float2> positionBuffer : register(u203, CALL_SET);
RWStructuredBuffer<float2> velocityBuffer : register(u204, CALL_SET);
RWStructuredBuffer<float> densityBuffer : register(u205, CALL_SET);
RWStructuredBuffer<float2> normalBuffer : register(u206, CALL_SET);
RWStructuredBuffer<float> curvatureBuffer : register(u207, CALL_SET);
RWStructuredBuffer<float2> forceDensityBuffer : register(u208, CALL_SET);
RWStructuredBuffer<float> nearDensityBuffer : register(u209, CALL_SET);
RWStructuredBuffer<float2> tempBuffer0 : register(u210, CALL_SET);
RWStructuredBuffer<float2> tempBuffer1 : register(u211, CALL_SET);
RWStructuredBuffer<float2> tempBuffer2 : register(u212, CALL_SET);
RWStructuredBuffer<float2> tempBuffer3 : register(u213, CALL_SET);
RWStructuredBuffer<float2> tempBuffer4 : register(u214, CALL_SET);
RWStructuredBuffer<float2> tempBuffer5 : register(u215, CALL_SET);
RWStructuredBuffer<float2> tempBuffer6 : register(u216, CALL_SET);
RWStructuredBuffer<float2> tempBuffer7 : register(u217, CALL_SET);



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
        nearDensityBuffer[index] = 0.0f;
        tempBuffer0[index] = float2(0, 0);
        tempBuffer1[index] = float2(0, 0);
        tempBuffer2[index] = float2(0, 0);
        tempBuffer3[index] = float2(0, 0);
        tempBuffer4[index] = float2(0, 0);
        tempBuffer5[index] = float2(0, 0);
        tempBuffer6[index] = float2(0, 0);
        tempBuffer7[index] = float2(0, 0);
    }
}