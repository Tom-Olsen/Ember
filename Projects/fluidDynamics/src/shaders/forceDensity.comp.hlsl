#include "computePushConstant.hlsli"
#include "hashGridFunctions.hlsli"
#include "math.hlsli"
#include "smoothingKernals.hlsli"



cbuffer Values : register(b0)
{
    // General parameters:
    int particleCount;
    int useGridOptimization;
    float gridRadius;
    
    // Physics:
    float viscosity;
    float mass;
    float gravity;
    
    // Sph parameters:
    float effectRadius;
    float targetDensity;
    float pressureMultiplier;
    
    // Attractor:
    int attractorState;
    float attractorRadius;
    float attractorStrength;
    float2 attractorPoint;
};
StructuredBuffer<uint> cellKeyBuffer : register(t1);
StructuredBuffer<uint> startIndexBuffer : register(t2);
StructuredBuffer<float2> positionBuffer : register(t3);
StructuredBuffer<float2> velocityBuffer : register(t4);
StructuredBuffer<float> densityBuffer : register(t5);
RWStructuredBuffer<float2> forceDensityBuffer : register(u6);



float Pressure(float density, float targetDensity, float pressureMultiplier)
{
    float densityError = density - targetDensity;
    return densityError * pressureMultiplier;
}



[numthreads(128, 1, 1)]
void main(uint3 threadID : SV_DispatchThreadID)
{
    int index = int(threadID.x);
    if (index < pc.threadCount.x)
    {
        forceDensityBuffer[index] = float2(0, 0);
        float particlePressure = Pressure(densityBuffer[index], targetDensity, pressureMultiplier);
        float2 particlePos = positionBuffer[index];
        float2 particleVel = velocityBuffer[index];
        
		// Internal interactions (particle-particle):
        if (useGridOptimization)
        {// With hash grid optimization:
            int2 particleCell = Cell(particlePos, gridRadius);
            for (int i = 0; i < 9; i++)
            {
                int2 neighbourCell = particleCell + offsets[i];
                int neighbourCellHash = CellHash(neighbourCell);
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
                    
                    float2 otherPos = positionBuffer[otherIndex];
                    float2 otherVel = velocityBuffer[otherIndex];
                    float2 offset = particlePos - otherPos;
                    float r = length(offset);
                    if (r < effectRadius)
                    {
						// Pressure force density:
                        float phi = 2.0f * math_PI * Random_FromTime(pc.time);
                        float2 dir = (r < 1e-8f) ? float2(cos(phi), sin(phi)) : offset / r;
                        float otherParticlePressure = Pressure(densityBuffer[otherIndex], targetDensity, pressureMultiplier);
                        float sharedPressure = 0.5f * (particlePressure + otherParticlePressure);
                        forceDensityBuffer[index] += -mass * sharedPressure * SmoothingKernal_DSpiky(r, dir, effectRadius) / densityBuffer[otherIndex];

						// Viscosity force density:
                        float2 velocityDiff = otherVel - particleVel;
                        forceDensityBuffer[index] += (mass * SmoothingKernal_DDViscos(r, effectRadius) / densityBuffer[otherIndex]) * velocityDiff;
                    }

                    otherIndex++;
                }
            }
        }
        else
        {// Naive iteration over all particles:
            for (int i = 0; i < particleCount; i++)
            {
                if (i == index)
                    continue;
            
                float2 offset = positionBuffer[index] - positionBuffer[i];
                float r = length(offset);
                if (r < effectRadius)
                {
                    // Pressure force density:
                    float phi = 2.0f * math_PI * Random_FromTime(pc.time);
                    float2 dir = (r < 1e-8f) ? float2(cos(phi), sin(phi)) : offset / r;
                    float otherParticlePressure = Pressure(densityBuffer[i], targetDensity, pressureMultiplier);
                    float sharedPressure = 0.5f * (particlePressure + otherParticlePressure);
                    forceDensityBuffer[index] += -mass * sharedPressure * SmoothingKernal_DSpiky(r, dir, effectRadius) / densityBuffer[i];
                
                    // Viscosity force density:
                    float2 velocityDiff = velocityBuffer[i] - velocityBuffer[index];
                    forceDensityBuffer[index] += viscosity * (mass * SmoothingKernal_DDViscos(r, effectRadius) / densityBuffer[i]) * velocityDiff;
                }
            };
        }
        // External interactions:
        {
            // Gravity force density:
            forceDensityBuffer[index] += densityBuffer[index] * float2(0.0f, -gravity);
            
            // External force density:
            float2 offset = attractorPoint - positionBuffer[index];
            float r = length(offset);
            if (r < attractorRadius && r > 1e-8f)
            {
                r /= attractorRadius;
                forceDensityBuffer[index] += attractorState * attractorStrength * densityBuffer[index] * offset * r * r;
            }
        }
    }
}