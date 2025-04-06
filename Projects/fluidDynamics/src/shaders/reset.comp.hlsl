#include "computePushConstant.hlsli"
#include "math.hlsli"



RWStructuredBuffer<float2> b_positions : register(u0);
cbuffer Values : register(b1)
{
    float cb_initialDistributionRadius;
};



[numthreads(64, 1, 1)]
void main(uint3 threadID : SV_DispatchThreadID)
{
    float phi = math_PI * (sqrt(5.0f) - 1.0f);
    
    uint index = threadID.x;
    if (index < pc.threadCount.x)
    {
        float r = index / (pc.threadCount.x - 1.0f) * cb_initialDistributionRadius;
        float theta = phi * index;
        b_positions[index].x = cos(theta) * r;
        b_positions[index].y = sin(theta) * r;
    }
}