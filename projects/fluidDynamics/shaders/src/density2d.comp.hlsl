#include "computePushConstant.hlsli"
#include "hashGridFunctions2d.hlsli"
#include "math.hlsli"
#include "smoothingKernals2d.hlsli"



cbuffer Values : register(b0)
{
    // particleCount = pc.threadCount.x
    uint hashGridSize; // ~2*particleCount
    uint useHashGridOptimization;
    float effectRadius;
    float mass;
};
StructuredBuffer<uint> cellKeyBuffer : register(t1);
StructuredBuffer<uint> startIndexBuffer : register(t2);
StructuredBuffer<float2> positionBuffer : register(t3);
RWStructuredBuffer<float> densityBuffer : register(u4);



[numthreads(128, 1, 1)]
void main(uint3 threadID : SV_DispatchThreadID)
{
    uint index = threadID.x;
    if (index < pc.threadCount.x)
    {
        densityBuffer[index] = 0;
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
                    float2 offset = particlePos - positionBuffer[otherIndex];
                    float r = length(offset);
                    if (r < effectRadius)
                        densityBuffer[index] += mass * SmoothingKernal_Poly6(r, effectRadius);
                    otherIndex++;
                }
            }
        }
        else
        {
            for (uint i = 0; i < pc.threadCount.x; i++)
            {
                float2 offset = particlePos - positionBuffer[i];
                float r = length(offset);
                if (r < effectRadius)
                    densityBuffer[index] += mass * SmoothingKernal_Poly6(r, effectRadius);
            };
        }
    }
}