#include "computeShaderCommon.hlsli"
#include "hashGridFunctions2d.hlsli"
#include "smoothingKernals2d.hlsli"



cbuffer Values : register(b300, SHADER_SET)
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
    int attractorState;
    float attractorRadius;
    float attractorStrength;
    float2 attractorPoint;
};
StructuredBuffer<uint> cellKeyBuffer : register(t100, CALL_SET);
StructuredBuffer<uint> startIndexBuffer : register(t101, CALL_SET);
StructuredBuffer<float2> positionBuffer : register(t102, CALL_SET);
StructuredBuffer<float2> velocityBuffer : register(t103, CALL_SET);
StructuredBuffer<float> densityBuffer : register(t104, CALL_SET);
StructuredBuffer<float2> normalBuffer : register(t105, CALL_SET);
StructuredBuffer<float> curvatureBuffer : register(t106, CALL_SET);
RWStructuredBuffer<float2> forceDensityBuffer : register(u200, CALL_SET);



float Pressure(float density, float targetDensity, float pressureMultiplier)
{
    float densityError = density - targetDensity;
    return densityError * pressureMultiplier;
}
float2 OverlapDirection(uint index, uint otherIndex, float time)
{
    float phi = 2.0f * math_PI * Random01(index, otherIndex, time);
    float2 dir = float2(cos(phi), sin(phi));
    return (index < otherIndex) ? dir : -dir;
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
        if (useHashGridOptimization)
        {
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
                    
                    float2 offset = particlePos - positionBuffer[otherIndex];
                    float r = length(offset);
                    if (r < effectRadius)
                    {
                        // Pressure force density:
                        float2 dir = (r < 1e-8f) ? OverlapDirection(index, otherIndex, pc.time) : offset / r;
                        float otherParticlePressure = Pressure(densityBuffer[otherIndex], targetDensity, pressureMultiplier);
                        float sharedPressure = 0.5f * (particlePressure + otherParticlePressure);
                        forceDensityBuffer[index] += -mass * sharedPressure * SmoothingKernal_DSpiky(r, dir, effectRadius) / densityBuffer[otherIndex];

                        // Viscosity force density:
                        float2 velocityDiff = velocityBuffer[otherIndex] - velocityBuffer[index];
                        forceDensityBuffer[index] += viscosity * (mass * SmoothingKernal_DDViscos(r, effectRadius) / densityBuffer[otherIndex]) * velocityDiff;
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
                    float2 dir = (r < 1e-8f) ? OverlapDirection(index, i, pc.time) : offset / r;
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