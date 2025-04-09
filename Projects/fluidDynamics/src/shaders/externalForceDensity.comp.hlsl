#include "computePushConstant.hlsli"
#include "math.hlsli"
#include "smoothingKernals.hlsli"



cbuffer Values : register(b0)
{
    int cb_particleCount;
    int cb_attractorState;
    float cb_attractorRadius;
    float cb_attractorStrength;
    float2 cb_attractorPoint;
};
StructuredBuffer<float2> b_positions : register(t1);
StructuredBuffer<float> b_densities : register(t2);
RWStructuredBuffer<float2> b_forceDensities : register(u3);



[numthreads(64, 1, 1)]
void main(uint3 threadID : SV_DispatchThreadID)
{
    int index = int(threadID.x);
    if (index < pc.threadCount.x)
    {
        for (int i = 0; i < cb_particleCount; i++)
        {
            float2 offset = cb_attractorPoint - b_positions[index];
            float r = length(offset);
            if (r < cb_attractorRadius)
                b_forceDensities[index] += cb_attractorState * cb_attractorStrength * b_densities[index] * offset * r * r;
        };
    }
}