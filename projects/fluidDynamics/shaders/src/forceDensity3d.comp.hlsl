#include "computePushConstant.hlsli"
#include "hashGridFunctions3d.hlsli"
#include "math.hlsli"
#include "smoothingKernals3d.hlsli"



cbuffer Values : register(b0)
{
    // General parameters:
    uint particleCount;
    uint useGridOptimization;
    float gridRadius;
    
    // Physics:
    float viscosity;
    float mass;
    float gravity;
    float surfaceTension;
    
    // Sph parameters:
    float effectRadius;
    float targetDensity;
    float pressureMultiplier;
};
StructuredBuffer<uint> cellKeyBuffer : register(t1);
StructuredBuffer<uint> startIndexBuffer : register(t2);
StructuredBuffer<float3> positionBuffer : register(t3);
StructuredBuffer<float3> velocityBuffer : register(t4);
StructuredBuffer<float> densityBuffer : register(t5);
StructuredBuffer<float3> normalBuffer : register(t6);
StructuredBuffer<float> curvatureBuffer : register(t7);
RWStructuredBuffer<float3> forceDensityBuffer : register(u8);



float Pressure(float density, float targetDensity, float pressureMultiplier)
{
    float densityError = density - targetDensity;
    return densityError * pressureMultiplier;
}



[numthreads(128, 1, 1)]
void main(uint3 threadID : SV_DispatchThreadID)
{
    uint index = threadID.x;
    if (index < pc.threadCount.x)
    {
        forceDensityBuffer[index] = float3(0, 0, 0);
        float particlePressure = Pressure(densityBuffer[index], targetDensity, pressureMultiplier);
        float3 particlePos = positionBuffer[index];
        float3 particleVel = velocityBuffer[index];
        
		// Particle-Particle interaction forces:
        if (useGridOptimization)
        {// With hash grid optimization:
            int3 particleCell = Cell(particlePos, gridRadius);
            for (uint i = 0; i < 27; i++)
            {
                int3 neighbourCell = particleCell + offsets[i];
                uint neighbourCellHash = CellHash(neighbourCell);
                uint neighbourCellKey = CellKey(neighbourCellHash, particleCount);
                uint otherIndex = startIndexBuffer[neighbourCellKey];

                while (otherIndex < particleCount) // at most as many iterations as there are particles.
                {
                    // Skip self interaction:
                    if (otherIndex == index)
                    {
                        otherIndex++;
                        continue;
                    }
                    
                    uint otherCellKey = cellKeyBuffer[otherIndex];
                    if (otherCellKey != neighbourCellKey)	// found first particle that is in a different cell => done.
                        break;
                    
                    float3 otherPos = positionBuffer[otherIndex];
                    float3 otherVel = velocityBuffer[otherIndex];
                    float3 offset = particlePos - otherPos;
                    float r = length(offset);
                    if (r < effectRadius)
                    {
						// Pressure force density:
                        float phi = 2.0f * math_PI * Random_FromTime(pc.time);
                        float3 dir = (r < 1e-8f) ? float3(cos(phi), sin(phi), 0) : offset / r;  // TODO: use fibonacci sphere to map random number to point on sphere.
                        float otherParticlePressure = Pressure(densityBuffer[otherIndex], targetDensity, pressureMultiplier);
                        float sharedPressure = 0.5f * (particlePressure + otherParticlePressure);
                        forceDensityBuffer[index] += -mass * sharedPressure * SmoothingKernal_DSpiky(r, dir, effectRadius) / densityBuffer[otherIndex];

						// Viscosity force density:
                        float3 velocityDiff = otherVel - particleVel;
                        forceDensityBuffer[index] += (mass * SmoothingKernal_DDViscos(r, effectRadius) / densityBuffer[otherIndex]) * velocityDiff;
                    }

                    otherIndex++;
                }
            }
        }
        else
        {// Naive iteration over all particles:
            for (uint i = 0; i < particleCount; i++)
            {
                // Skip self interaction:
                if (i == index)
                    continue;
            
                float3 offset = positionBuffer[index] - positionBuffer[i];
                float r = length(offset);
                if (r < effectRadius)
                {
                    // Pressure force density:
                    float phi = 2.0f * math_PI * Random_FromTime(pc.time);
                    float3 dir = (r < 1e-8f) ? float3(cos(phi), sin(phi), 0) : offset / r; // TODO: use fibonacci sphere to map random number to point on sphere.
                    float otherParticlePressure = Pressure(densityBuffer[i], targetDensity, pressureMultiplier);
                    float sharedPressure = 0.5f * (particlePressure + otherParticlePressure);
                    forceDensityBuffer[index] += -mass * sharedPressure * SmoothingKernal_DSpiky(r, dir, effectRadius) / densityBuffer[i];
                
                    // Viscosity force density:
                    float3 velocityDiff = velocityBuffer[i] - velocityBuffer[index];
                    forceDensityBuffer[index] += viscosity * (mass * SmoothingKernal_DDViscos(r, effectRadius) / densityBuffer[i]) * velocityDiff;
                }
            };
        }
        // Self interaction forces:
        {
            // Gravity force density:
            forceDensityBuffer[index] += densityBuffer[index] * float3(0.0f, 0.0f, -gravity);
            
            // Surface tension:
            if (curvatureBuffer[index] != 0)
                forceDensityBuffer[index] -= surfaceTension * curvatureBuffer[index] * normalBuffer[index];
        }
    }
}