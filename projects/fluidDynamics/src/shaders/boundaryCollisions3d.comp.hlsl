#include "computePushConstant.hlsli"



cbuffer Values : register(b0)
{
    float collisionDampening;
    float3 min;
    float3 max;
};
RWStructuredBuffer<float3> positionBuffer : register(u1);
RWStructuredBuffer<float3> velocityBuffer : register(u2);



[numthreads(128, 1, 1)]
void main(uint3 threadID : SV_DispatchThreadID)
{
    const float epsilon = 1e-4f;
    uint index = threadID.x;
    
    if (index < pc.threadCount.x)
    {
        if (positionBuffer[index].x < min.x)
        {
            positionBuffer[index].x = min.x + epsilon;
            velocityBuffer[index].x *= -collisionDampening;
        }
        if (positionBuffer[index].x > max.x)
        {
            positionBuffer[index].x = max.x - epsilon;
            velocityBuffer[index].x *= -collisionDampening;
        }
        if (positionBuffer[index].y < min.y)
        {
            positionBuffer[index].y = min.y + epsilon;
            velocityBuffer[index].y *= -collisionDampening;
        }
        if (positionBuffer[index].y > max.y)
        {
            positionBuffer[index].y = max.y - epsilon;
            velocityBuffer[index].y *= -collisionDampening;
        }
        if (positionBuffer[index].z < min.z)
        {
            positionBuffer[index].z = min.z + epsilon;
            velocityBuffer[index].z *= -collisionDampening;
        }
        if (positionBuffer[index].z > max.z)
        {
            positionBuffer[index].z = max.z - epsilon;
            velocityBuffer[index].z *= -collisionDampening;
        }
    }
}