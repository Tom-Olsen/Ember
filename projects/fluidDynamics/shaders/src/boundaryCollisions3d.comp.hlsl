#include "computeShaderCommon.hlsli"



cbuffer Values : register(b300, SHADER_SET)
{
    float collisionDampening;
    float3 min;
    float3 max;
    float4x4 rotation;
    float4x4 inverseRotation;
};
RWStructuredBuffer<float3> positionBuffer : register(u200, CALL_SET);
RWStructuredBuffer<float3> velocityBuffer : register(u201, CALL_SET);



[numthreads(128, 1, 1)]
void main(uint3 threadID : SV_DispatchThreadID)
{
    const float boundaryOffset = 1e-4f;
    uint index = threadID.x;
    
    if (index < pc.threadCount.x)
    {
        // Transform world to local:
        float3 center = 0.5f * (min + max);
        float3 localPosition = center + mul(inverseRotation, float4(positionBuffer[index] - center, 0.0f)).xyz;
        float3 localVelocity = mul(inverseRotation, float4(velocityBuffer[index], 0.0f)).xyz;

        // Resolve collisions in local frame:
        bool collided = false;
        if (localPosition.x < min.x)
        {
            localPosition.x = min.x + boundaryOffset;
            localVelocity.x *= -collisionDampening;
            collided = true;
        }
        if (localPosition.x > max.x)
        {
            localPosition.x = max.x - boundaryOffset;
            localVelocity.x *= -collisionDampening;
            collided = true;
        }
        if (localPosition.y < min.y)
        {
            localPosition.y = min.y + boundaryOffset;
            localVelocity.y *= -collisionDampening;
            collided = true;
        }
        if (localPosition.y > max.y)
        {
            localPosition.y = max.y - boundaryOffset;
            localVelocity.y *= -collisionDampening;
            collided = true;
        }
        if (localPosition.z < min.z)
        {
            localPosition.z = min.z + boundaryOffset;
            localVelocity.z *= -collisionDampening;
            collided = true;
        }
        if (localPosition.z > max.z)
        {
            localPosition.z = max.z - boundaryOffset;
            localVelocity.z *= -collisionDampening;
            collided = true;
        }

        // Transform local to world and write back:
        if (collided)
        {
            positionBuffer[index] = center + mul(rotation, float4(localPosition - center, 0.0f)).xyz;
            velocityBuffer[index] = mul(rotation, float4(localVelocity, 0.0f)).xyz;
        }
    }
}