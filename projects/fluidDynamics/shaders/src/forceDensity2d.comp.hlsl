#include "computePushConstant.hlsli"
#include "hashGridFunctions2d.hlsli"
#include "math.hlsli"
#include "smoothingKernals2d.hlsli"



cbuffer Values : register(b0)
{
    // General parameters:
    // particleCount = pc.threadCount.x
    uint hashGridSize; // ~2*particleCount
    uint useHashGridOptimization;
    
    // Physics:
    float viscosity;
    float mass;
    float gravity;
    float surfaceTension;
    
    // Sph parameters:
    float effectRadius;
    float targetDensity;
    float pressureMultiplier;
    
    // Attractor:
    uint attractorState;
    float attractorRadius;
    float attractorStrength;
    float2 attractorPoint;
};
StructuredBuffer<uint> cellKeyBuffer : register(t1);
StructuredBuffer<uint> startIndexBuffer : register(t2);
StructuredBuffer<float2> positionBuffer : register(t3);
StructuredBuffer<float2> velocityBuffer : register(t4);
StructuredBuffer<float> densityBuffer : register(t5);
StructuredBuffer<float2> normalBuffer : register(t6);
StructuredBuffer<float> curvatureBuffer : register(t7);
RWStructuredBuffer<float2> forceDensityBuffer : register(u8);



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
        forceDensityBuffer[index] = float2(0, 0);
        float particlePressure = Pressure(densityBuffer[index], targetDensity, pressureMultiplier);
        float2 particlePos = positionBuffer[index];
        float2 particleVel = velocityBuffer[index];
        
		// Particle-Particle interaction forces:
        if (useHashGridOptimization)
        { // With hash grid optimization:
            int2 particleCell = HashGrid2d_Cell(particlePos, effectRadius);
            for (uint i = 0; i < 9; i++)
            {
                int2 neighbourCell = particleCell + offsets[i];
                uint cellKey = HashGrid2d_GetCellKey(neighbourCell, hashGridSize);
                uint otherIndex = HashGrid2d_GetStartIndex(neighbourCell, hashGridSize, startIndexBuffer);
                
				// Skip empty cells:
                if (otherIndex == uint(-1) || otherIndex >= pc.threadCount.x)
                    continue;
                
                while (otherIndex < pc.threadCount.x && cellKeyBuffer[otherIndex] == cellKey)
                {
                    // Skip self interaction:
                    if (otherIndex == index)
                    {
                        otherIndex++;
                        continue;
                    }
                    
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
        {
            for (uint i = 0; i < pc.threadCount.x; i++)
            {
                // Skip self interaction:
                if (i == index)
                    continue;
            
                float2 offset = particlePos - positionBuffer[i];
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
        // Self interaction forces:
        {
            // Gravity force density:
            forceDensityBuffer[index] += densityBuffer[index] * float2(0.0f, -gravity);
            
            // Surface tension:
            if (curvatureBuffer[index] != 0)
                forceDensityBuffer[index] -= surfaceTension * curvatureBuffer[index] * normalBuffer[index];
            
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