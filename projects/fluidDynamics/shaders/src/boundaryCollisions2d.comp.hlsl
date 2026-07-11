#include "computeShaderCommon.hlsli"



cbuffer Values : register(b300, SHADER_SET)
{
    float collisionDampening;
    float3 boundsMin;
    float3 boundsMax;
};
RWStructuredBuffer<float2> positionBuffer : register(u200, CALL_SET);
RWStructuredBuffer<float2> velocityBuffer : register(u201, CALL_SET);



[numthreads(128, 1, 1)]
void main(uint3 threadID : SV_DispatchThreadID)
{
    const float boundaryOffset = 1e-4f;
    uint index = threadID.x;
    
    if (index < pc.threadCount.x)
    {
        if (positionBuffer[index].x < boundsMin.x)
        {
            positionBuffer[index].x = boundsMin.x + boundaryOffset;
            velocityBuffer[index].x *= -collisionDampening;
        }
        if (positionBuffer[index].x > boundsMax.x)
        {
            positionBuffer[index].x = boundsMax.x - boundaryOffset;
            velocityBuffer[index].x *= -collisionDampening;
        }
        if (positionBuffer[index].y < boundsMin.y)
        {
            positionBuffer[index].y = boundsMin.y + boundaryOffset;
            velocityBuffer[index].y *= -collisionDampening;
        }
        if (positionBuffer[index].y > boundsMax.y)
        {
            positionBuffer[index].y = boundsMax.y - boundaryOffset;
            velocityBuffer[index].y *= -collisionDampening;
        }
    }
}