#include "computePushConstant.hlsli"
#include "hashGridFunctions3d.hlsli"
#include "math.hlsli"
#include "smoothingKernals3d.hlsli"



cbuffer Values : register(b0)
{
    uint particleCount;
    uint useGridOptimization;
    float gridRadius;
    float effectRadius;
    float mass;
};
StructuredBuffer<uint> cellKeyBuffer : register(t1);
StructuredBuffer<uint> startIndexBuffer : register(t2);
StructuredBuffer<float3> positionBuffer : register(t3);
RWStructuredBuffer<float> densityBuffer : register(u4);



[numthreads(128, 1, 1)]
void main(uint3 threadID : SV_DispatchThreadID)
{
    uint index = threadID.x;
    if (index < pc.threadCount.x)
    {
        if (useGridOptimization)
        {// With hash grid optimization:
            float3 particlePos = positionBuffer[index];
            int3 particleCell = Cell(particlePos, gridRadius);
            
            densityBuffer[index] = 0;
            for (uint i = 0; i < 27; i++)
            {
                int3 neighbourCell = particleCell + offsets[i];
                uint neighbourCellHash = CellHash(neighbourCell);
                uint neighbourCellKey = CellKey(neighbourCellHash, particleCount);
                uint otherIndex = startIndexBuffer[neighbourCellKey];
            
                while (otherIndex < particleCount) // at most as many iterations as there are particles.
                {
                    uint otherCellKey = cellKeyBuffer[otherIndex];
                    if (otherCellKey != neighbourCellKey)	// found first particle that is in a different cell => done.
                        break;
            
                    float3 otherPos = positionBuffer[otherIndex];
                    float3 offset = particlePos - otherPos;
                    float r = length(offset);
                    if (r < effectRadius)
                        densityBuffer[index] += mass * SmoothingKernal_Poly6(r, effectRadius);
            
                    otherIndex++;
                }
            }
        }
        else
        {// Naive iteration over all particles:
            densityBuffer[index] = 0;
            for (uint i = 0; i < particleCount; i++)
            {
                float3 offset = positionBuffer[index] - positionBuffer[i];
                float r = length(offset);
                if (r < effectRadius)
                    densityBuffer[index] += mass * SmoothingKernal_Poly6(r, effectRadius);
            };
        }
    }
}