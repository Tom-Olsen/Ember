#include "computeShaderCommon.hlsli"



cbuffer Values : register(b300, SHADER_SET)
{
    float collisionDampening;
    float3 min;
    float3 max;
};
RWStructuredBuffer<float3> positionBuffer : register(u200, SHADER_SET);
RWStructuredBuffer<float3> velocityBuffer : register(u201, SHADER_SET);



[numthreads(128, 1, 1)]
void main(uint3 threadID : SV_DispatchThreadID)
{
    const float boundaryOffset = 1e-4f;
    uint index = threadID.x;
    
    if (index < pc.threadCount.x)
    {
        if (positionBuffer[index].x < min.x)
        {
            positionBuffer[index].x = min.x + boundaryOffset;
            velocityBuffer[index].x *= -collisionDampening;
        }
        if (positionBuffer[index].x > max.x)
        {
            positionBuffer[index].x = max.x - boundaryOffset;
            velocityBuffer[index].x *= -collisionDampening;
        }
        if (positionBuffer[index].y < min.y)
        {
            positionBuffer[index].y = min.y + boundaryOffset;
            velocityBuffer[index].y *= -collisionDampening;
        }
        if (positionBuffer[index].y > max.y)
        {
            positionBuffer[index].y = max.y - boundaryOffset;
            velocityBuffer[index].y *= -collisionDampening;
        }
        if (positionBuffer[index].z < min.z)
        {
            positionBuffer[index].z = min.z + boundaryOffset;
            velocityBuffer[index].z *= -collisionDampening;
        }
        if (positionBuffer[index].z > max.z)
        {
            positionBuffer[index].z = max.z - boundaryOffset;
            velocityBuffer[index].z *= -collisionDampening;
        }
    }
}