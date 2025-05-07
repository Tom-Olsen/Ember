#include "computePushConstant.hlsli"
#include "hashGridFunctions3d.hlsli"
#include "math.hlsli"
#include "smoothingKernals3d.hlsli"



cbuffer Values : register(b0)
{
    int particleCount;
    int useGridOptimization;
    float gridRadius;
    float effectRadius;
    float mass;
};
StructuredBuffer<uint> cellKeyBuffer : register(t1);
StructuredBuffer<uint> startIndexBuffer : register(t2);
StructuredBuffer<float3> positionBuffer : register(t3);
StructuredBuffer<float> densityBuffer : register(t4);
RWStructuredBuffer<float3> normalBuffer : register(u5);
RWStructuredBuffer<float> curvatureBuffer : register(u6);



[numthreads(128, 1, 1)]
void main(uint3 threadID : SV_DispatchThreadID)
{
    uint index = threadID.x;
    if (index < pc.threadCount.x)
    {
        if (useGridOptimization)
        { // With hash grid optimization:
            float3 particlePos = positionBuffer[index];
            int3 particleCell = Cell(particlePos, gridRadius);
            
            normalBuffer[index] = 0;
            curvatureBuffer[index] = 0;
            for (int i = 0; i < 27; i++)
            {
                int3 neighbourCell = particleCell + offsets[i];
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
            
                    float3 otherPos = positionBuffer[otherIndex];
                    float3 offset = particlePos - otherPos;
                    float r = length(offset);
                    if (r < effectRadius)
                    {
                        float3 dir = offset / r;
                        float c = mass / densityBuffer[otherIndex];
                        normalBuffer[index] += c * SmoothingKernal_DPoly6(r, dir, effectRadius);
                        curvatureBuffer[index] += c * SmoothingKernal_DDPoly6(r, effectRadius);
                    }
            
                    otherIndex++;
                }
                float normalLength = length(normalBuffer[index]);
                if (normalLength > 1e-2f)
                    curvatureBuffer[index] /= normalLength;
                else
                    curvatureBuffer[index] = 0;

            }
        }
        else
        { // Naive iteration over all particles:
            normalBuffer[index] = 0;    
            curvatureBuffer[index] = 0;
            for (int i = 0; i < particleCount; i++)
            {
                // Skip self interaction:
                if (i == index)
                    continue;
                
                float3 offset = positionBuffer[index] - positionBuffer[i];
                float r = length(offset);
                if (r < effectRadius)
                {
                    float3 dir = offset / r;
                    float c = mass / densityBuffer[i];
                    normalBuffer[index] += c * SmoothingKernal_DPoly6(r, dir, effectRadius);
                    curvatureBuffer[index] += c * SmoothingKernal_DDPoly6(r, effectRadius);
                }
            };
            float normalLength = length(normalBuffer[index]);
            if (normalLength > 1e-2f)
                curvatureBuffer[index] /= normalLength;
            else
                curvatureBuffer[index] = 0;
        }
    }
}