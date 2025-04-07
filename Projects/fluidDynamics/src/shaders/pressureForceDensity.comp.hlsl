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
StructuredBuffer<float> b_densities : register(t2);
RWStructuredBuffer<float2> b_forceDensities : register(u3);



float Pressure(float density, float targetDensity, float pressureMultiplier)
{
    float densityError = density - targetDensity;
    return densityError * pressureMultiplier;
}



[numthreads(64, 1, 1)]
void main(uint3 threadID : SV_DispatchThreadID)
{
    uint index = threadID.x;
    if (index < pc.threadCount.x)
    {
        b_forceDensities[index] = float2(0, 0);
        float particlePressure = Pressure(b_densities[index], cb_targetDensity, cb_pressureMultiplier);
        for (int i = 0; i < cb_particleCount; i++)
        {
            if (i == index)
                continue;
            
            float2 offset = b_positions[index] - b_positions[i];
            float r = length(offset);
            if (r < cb_effectRadius)
            {
                float2 dir = (r < 1e-8f) ? float2(cos(Random_FromTime(pc.time)), sin(Random_FromTime(pc.time))) : offset / r;
                float otherParticlePressure = Pressure(b_densities[i], cb_targetDensity, cb_pressureMultiplier);
                float sharedPressure = 0.5f * (particlePressure + otherParticlePressure);
                b_forceDensities[index] += -cb_mass * sharedPressure * SmoothingKernal_DSpiky(r, dir, cb_effectRadius) / b_densities[i];
            }
        };
    }
}