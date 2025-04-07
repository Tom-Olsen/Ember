#include "computePushConstant.hlsli"
#include "math.hlsli"
#include "smoothingKernals.hlsli"



cbuffer Values : register(b0)
{
    int cb_particleCount;
    float cb_effectRadius;
    float cb_mass;
};
// change to positions buffer to StructuredBuffer as it is read only.
RWStructuredBuffer<float2> b_positions : register(u1);
RWStructuredBuffer<float> b_densities : register(u2);



[numthreads(64, 1, 1)]
void main(uint3 threadID : SV_DispatchThreadID)
{
    uint index = threadID.x;
    if (index < pc.threadCount.x)
    {
        b_densities[index] = 0;
        for (int i = 0; i < cb_particleCount; i++)
        {
            float2 offset = b_positions[index] - b_positions[i];
            float r = length(offset);
            if (r < cb_effectRadius)
                b_densities[index] += cb_mass * SmoothingKernal_Poly6(r, cb_effectRadius);
        };
    }
}