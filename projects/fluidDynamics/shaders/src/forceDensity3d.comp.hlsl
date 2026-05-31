#include "computeShaderCommon.hlsli"
#include "hashGridFunctions3d.hlsli"
#include "smoothingKernals3d.hlsli"



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
    //int attractorState;
    //float attractorRadius;
    //float attractorStrength;
    //float3 attractorPoint;
};
StructuredBuffer<uint> cellKeyBuffer : register(t100, CALL_SET);
StructuredBuffer<uint> startIndexBuffer : register(t101, CALL_SET);
StructuredBuffer<float3> positionBuffer : register(t102, CALL_SET);
StructuredBuffer<float3> velocityBuffer : register(t103, CALL_SET);
StructuredBuffer<float> densityBuffer : register(t104, CALL_SET);
StructuredBuffer<float3> normalBuffer : register(t105, CALL_SET);
StructuredBuffer<float> curvatureBuffer : register(t106, CALL_SET);
RWStructuredBuffer<float3> forceDensityBuffer : register(u200, CALL_SET);



float Pressure(float density, float targetDensity, float pressureMultiplier)
{
    float densityError = density - targetDensity;
    return densityError * pressureMultiplier;
}
float3 OverlapDirection(uint index, uint otherIndex, float time)
{
    float z = 1.0f - 2.0f * Random01(index, otherIndex, time);
    float phi = 2.0f * math_PI * Random01(index, otherIndex, 1u, time);
    float rxy = sqrt(max(0.0f, 1.0f - z * z));
    float3 dir = float3(rxy * cos(phi), rxy * sin(phi), z);
    return (index < otherIndex) ? dir : -dir;
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
        if (useHashGridOptimization)
        {
            int3 particleCell = HashGrid3d_Cell(particlePos, effectRadius);
            for (uint i = 0; i < 27; i++)
            {
                int3 neighbourCell = particleCell + offsets[i];
                uint cellKey = HashGrid3d_GetCellKey(neighbourCell, hashGridSize);
                uint otherIndex = HashGrid3d_GetStartIndex(neighbourCell, hashGridSize, startIndexBuffer);

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

                    float3 offset = particlePos - positionBuffer[otherIndex];
                    float r = length(offset);
                    if (r < effectRadius)
                    {
                        // Pressure force density:
                        float3 dir = (r < 1e-8f) ? OverlapDirection(index, otherIndex, pc.time) : offset / r;
                        float otherParticlePressure = Pressure(densityBuffer[otherIndex], targetDensity, pressureMultiplier);
                        float sharedPressure = 0.5f * (particlePressure + otherParticlePressure);
                        forceDensityBuffer[index] += -mass * sharedPressure * SmoothingKernal_DSpiky(r, dir, effectRadius) / densityBuffer[otherIndex];

                        // Viscosity force density:
                        float3 velocityDiff = velocityBuffer[otherIndex] - velocityBuffer[index];
                        forceDensityBuffer[index] += viscosity * mass * velocityDiff * (SmoothingKernal_DDViscos(r, effectRadius) / densityBuffer[otherIndex]);
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

                float3 offset = particlePos - positionBuffer[i];
                float r = length(offset);
                if (r < effectRadius)
                {
                    // Pressure force density:
                    float3 dir = (r < 1e-8f) ? OverlapDirection(index, i, pc.time) : offset / r;
                    float otherParticlePressure = Pressure(densityBuffer[i], targetDensity, pressureMultiplier);
                    float sharedPressure = 0.5f * (particlePressure + otherParticlePressure);
                    forceDensityBuffer[index] += -mass * sharedPressure * SmoothingKernal_DSpiky(r, dir, effectRadius) / densityBuffer[i];

                    // Viscosity force density:
                    float3 velocityDiff = velocityBuffer[i] - velocityBuffer[index];
                    forceDensityBuffer[index] += viscosity * mass * velocityDiff * (SmoothingKernal_DDViscos(r, effectRadius) / densityBuffer[i]);
                }
            };
        }
        // Self interaction forces:
        {
            // Gravity force density:
            forceDensityBuffer[index] += densityBuffer[index] * float3(0.0f, 0.0f, -gravity);

            // Surface tension:
            forceDensityBuffer[index] += surfaceTension * curvatureBuffer[index] * normalBuffer[index];

            // Attractor force density:
            //float3 offset = attractorPoint - positionBuffer[index];
            //float r = length(offset);
            //if (r < attractorRadius && r > 1e-8f)
            //{
            //    r /= attractorRadius;
            //    forceDensityBuffer[index] += attractorState * attractorStrength * densityBuffer[index] * offset * r * r;
            //}
        }
    }
}