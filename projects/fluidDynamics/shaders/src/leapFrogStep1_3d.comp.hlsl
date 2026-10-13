#include "computeShaderCommon.hlsli"



cbuffer CallValues : register(b301, CALL_SET)
{
    float dt;
};
StructuredBuffer<float3> sourcePositionBuffer : register(t102, CALL_SET);
StructuredBuffer<float3> sourceVelocityBuffer : register(t103, CALL_SET);
RWStructuredBuffer<float3> positionBuffer : register(u200, CALL_SET);
RWStructuredBuffer<float3> velocityBuffer : register(u201, CALL_SET);



[numthreads(128, 1, 1)]
void main(uint3 threadID : SV_DispatchThreadID)
{
    uint index = threadID.x;
    if (index < pc.threadCount.x)
    {
        positionBuffer[index] = sourcePositionBuffer[index] + 0.5f * dt * sourceVelocityBuffer[index];
        velocityBuffer[index] = sourceVelocityBuffer[index];
    }
}