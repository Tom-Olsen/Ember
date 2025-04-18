#include "computePushConstant.hlsli"



cbuffer Values : register(b0)
{
    float cb_gravity;
};
StructuredBuffer<float> b_densities : register(t1);
RWStructuredBuffer<float2> b_forceDensities : register(u2);



[numthreads(64, 1, 1)]
void main(uint3 threadID : SV_DispatchThreadID)
{
    int index = int(threadID.x);
    if (index < pc.threadCount.x)
    {
        b_forceDensities[index] += b_densities[index] * float2(0.0f, -cb_gravity);
    }
}