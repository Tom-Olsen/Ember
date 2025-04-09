#include "computePushConstant.hlsli"
#include "math.hlsli"
#include "smoothingKernals.hlsli"



cbuffer Values : register(b0)
{
    int cb_particleCount;
    float cb_effectRadius;
    float cb_mass;
    float cb_targetDensity;
    float cb_pressureMultiplier;
};
StructuredBuffer<float2> b_positions : register(t1);
StructuredBuffer<float2> b_velocities : register(t2);
StructuredBuffer<float> b_densities : register(t3);
RWStructuredBuffer<float2> b_forceDensities : register(u4);



[numthreads(64, 1, 1)]
void main(uint3 threadID : SV_DispatchThreadID)
{
    int index = int(threadID.x);
    if (index < pc.threadCount.x)
    {
        for (int i = 0; i < cb_particleCount; i++)
        {
            if (i == index)
                continue;
            
            float2 offset = b_positions[index] - b_positions[i];
            float r = length(offset);
            if (r < cb_effectRadius)
            {
                float2 velocityDiff = b_velocities[i] - b_velocities[index];
                b_forceDensities[index] += (cb_mass * SmoothingKernal_DDViscos(r, cb_effectRadius) / b_densities[i]) * velocityDiff;
            }
        };
    }
}