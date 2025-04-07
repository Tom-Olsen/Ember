#include "computePushConstant.hlsli"



cbuffer Values : register(b0)
{
    float cb_collisionDampening;
    float3 cb_min;
    float3 cb_max;
};
RWStructuredBuffer<float2> b_positions : register(u1);
RWStructuredBuffer<float2> b_velocities : register(u2);



[numthreads(64, 1, 1)]
void main(uint3 threadID : SV_DispatchThreadID)
{
    const float epsilon = 1e-4f;
    uint index = threadID.x;
    
    if (index < pc.threadCount.x)
    {
        if (b_positions[index].x < cb_min.x)
        {
            b_positions[index].x = cb_min.x + epsilon;
            b_velocities[index].x *= -cb_collisionDampening;
        }
        if (b_positions[index].x > cb_max.x)
        {
            b_positions[index].x = cb_max.x - epsilon;
            b_velocities[index].x *= -cb_collisionDampening;
        }
        if (b_positions[index].y < cb_min.y)
        {
            b_positions[index].y = cb_min.y + epsilon;
            b_velocities[index].y *= -cb_collisionDampening;
        }
        if (b_positions[index].y > cb_max.y)
        {
            b_positions[index].y = cb_max.y - epsilon;
            b_velocities[index].y *= -cb_collisionDampening;
        }
    }
}