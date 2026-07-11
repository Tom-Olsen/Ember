#include "computeShaderCommon.hlsli"



cbuffer Values : register(b300, SHADER_SET)
{
    float collisionDampening;
    float3 boundsMin;
    float3 boundsMax;
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
        float3 center = 0.5f * (boundsMin + boundsMax);
        float3 localPosition = center + mul(inverseRotation, float4(positionBuffer[index] - center, 0.0f)).xyz;
        float3 localVelocity = mul(inverseRotation, float4(velocityBuffer[index], 0.0f)).xyz;

        // Resolve collisions in local frame (abs ensures inward movement):
        bool collided = false;
        if (localPosition.x < boundsMin.x)
        {
            localPosition.x = boundsMin.x + boundaryOffset;
            localVelocity.x = abs(localVelocity.x) * collisionDampening;
            collided = true;
        }
        if (localPosition.x > boundsMax.x)
        {
            localPosition.x = boundsMax.x - boundaryOffset;
            localVelocity.x = -abs(localVelocity.x) * collisionDampening;
            collided = true;
        }
        if (localPosition.y < boundsMin.y)
        {
            localPosition.y = boundsMin.y + boundaryOffset;
            localVelocity.y = abs(localVelocity.y) * collisionDampening;
            collided = true;
        }
        if (localPosition.y > boundsMax.y)
        {
            localPosition.y = boundsMax.y - boundaryOffset;
            localVelocity.y = -abs(localVelocity.y) * collisionDampening;
            collided = true;
        }
        if (localPosition.z < boundsMin.z)
        {
            localPosition.z = boundsMin.z + boundaryOffset;
            localVelocity.z = abs(localVelocity.z) * collisionDampening;
            collided = true;
        }
        if (localPosition.z > boundsMax.z)
        {
            localPosition.z = boundsMax.z - boundaryOffset;
            localVelocity.z = -abs(localVelocity.z) * collisionDampening;
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