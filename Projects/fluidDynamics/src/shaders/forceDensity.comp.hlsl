#include "computePushConstant.hlsli"
#include "math.hlsli"
#include "smoothingKernals.hlsli"



cbuffer Values : register(b0)
{
    // General parameters:
    int cb_particleCount;
    
    // Physics:
    float cb_viscosity;
    float cb_mass;
    float cb_gravity;
    
    // Sph parameters:
    float cb_effectRadius;
    float cb_targetDensity;
    float cb_pressureMultiplier;
    
    // Attractor:
    int cb_attractorState;
    float cb_attractorRadius;
    float cb_attractorStrength;
    float2 cb_attractorPoint;
};
StructuredBuffer<float2> b_positions : register(t1);
StructuredBuffer<float2> b_velocities : register(t2);
StructuredBuffer<float> b_densities : register(t3);
RWStructuredBuffer<float2> b_forceDensities : register(u4);



float Pressure(float density, float targetDensity, float pressureMultiplier)
{
    float densityError = density - targetDensity;
    return densityError * pressureMultiplier;
}



[numthreads(64, 1, 1)]
void main(uint3 threadID : SV_DispatchThreadID)
{
    int index = int(threadID.x);
    if (index < pc.threadCount.x)
    {
        b_forceDensities[index] = float2(0, 0);
        float particlePressure = Pressure(b_densities[index], cb_targetDensity, cb_pressureMultiplier);
        
        // Internal interactions (particle-particle):
        for (int i = 0; i < cb_particleCount; i++)
        {
            if (i == index)
                continue;
            
            float2 offset = b_positions[index] - b_positions[i];
            float r = length(offset);
            if (r < cb_effectRadius)
            {
                // Pressure force density:
                float phi = 2.0f * math_PI * Random_FromTime(pc.time);
                float2 dir = (r < 1e-8f) ? float2(cos(phi), sin(phi)) : offset / r;
                float otherParticlePressure = Pressure(b_densities[i], cb_targetDensity, cb_pressureMultiplier);
                float sharedPressure = 0.5f * (particlePressure + otherParticlePressure);
                b_forceDensities[index] += -cb_mass * sharedPressure * SmoothingKernal_DSpiky(r, dir, cb_effectRadius) / b_densities[i];
                
                // Viscosity force density:
                float2 velocityDiff = b_velocities[i] - b_velocities[index];
                b_forceDensities[index] += cb_viscosity * (cb_mass * SmoothingKernal_DDViscos(r, cb_effectRadius) / b_densities[i]) * velocityDiff;
                
            }
        };
        
        // External interactions:
        {
            // Gravity force density:
            b_forceDensities[index] += b_densities[index] * float2(0.0f, -cb_gravity);
            
            // External force density:
            float2 offsetClick = cb_attractorPoint - b_positions[index];
            float rClick = length(offsetClick);
            if (rClick < cb_attractorRadius && rClick > 1e-8f)
            {
                rClick /= cb_attractorRadius;
                b_forceDensities[index] += cb_attractorState * cb_attractorStrength * b_densities[index] * offsetClick * rClick * rClick;
            }
        }
    }
}