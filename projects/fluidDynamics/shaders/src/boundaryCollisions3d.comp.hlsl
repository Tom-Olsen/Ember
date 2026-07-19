#include "computeShaderCommon.hlsli"



cbuffer CallValues : register(b301, CALL_SET)
{
    float3 boundsMin;
    float collisionDampening;
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
        // Transform simulation to bounds-local space:
        float3 center = 0.5f * (boundsMin + boundsMax);
        float3 positionFluid = center + mul(inverseRotation, float4(positionBuffer[index] - center, 0.0f)).xyz;
        float3 velocityFluid = mul(inverseRotation, float4(velocityBuffer[index], 0.0f)).xyz;

        // Resolve collisions in local frame (abs ensures inward movement):
        bool collided = false;
        if (positionFluid.x < boundsMin.x)
        {
            positionFluid.x = boundsMin.x + boundaryOffset;
            velocityFluid.x = abs(velocityFluid.x) * collisionDampening;
            collided = true;
        }
        if (positionFluid.x > boundsMax.x)
        {
            positionFluid.x = boundsMax.x - boundaryOffset;
            velocityFluid.x = -abs(velocityFluid.x) * collisionDampening;
            collided = true;
        }
        if (positionFluid.y < boundsMin.y)
        {
            positionFluid.y = boundsMin.y + boundaryOffset;
            velocityFluid.y = abs(velocityFluid.y) * collisionDampening;
            collided = true;
        }
        if (positionFluid.y > boundsMax.y)
        {
            positionFluid.y = boundsMax.y - boundaryOffset;
            velocityFluid.y = -abs(velocityFluid.y) * collisionDampening;
            collided = true;
        }
        if (positionFluid.z < boundsMin.z)
        {
            positionFluid.z = boundsMin.z + boundaryOffset;
            velocityFluid.z = abs(velocityFluid.z) * collisionDampening;
            collided = true;
        }
        if (positionFluid.z > boundsMax.z)
        {
            positionFluid.z = boundsMax.z - boundaryOffset;
            velocityFluid.z = -abs(velocityFluid.z) * collisionDampening;
            collided = true;
        }

        // Transform bounds-local to simulation space and write back:
        if (collided)
        {
            positionBuffer[index] = center + mul(rotation, float4(positionFluid - center, 0.0f)).xyz;
            velocityBuffer[index] = mul(rotation, float4(velocityFluid, 0.0f)).xyz;
        }
    }
}