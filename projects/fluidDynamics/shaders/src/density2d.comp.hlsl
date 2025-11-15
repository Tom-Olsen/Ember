#include "computePushConstant.hlsli"
#include "hashGridFunctions2d.hlsli"
#include "math.hlsli"
#include "smoothingKernals2d.hlsli"



cbuffer Values : register(b0)
{
    int particleCount;
    int hashGridSize; // ~2*particleCount
    int useGridOptimization;
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
        if (useGridOptimization)
        {
            int2 particleCell = HashGrid2d_Cell(particlePos, effectRadius);
            for (int i = 0; i < 9; i++)
            {
                int2 neighbourCell = particleCell + offsets[i];
                uint cellKey = HashGrid2d_GetCellKey(neighbourCell, hashGridSize);
                uint otherIndex = HashGrid2d_GetStartIndex(neighbourCell, hashGridSize, startIndexBuffer);
            
				// Skip empty cells:
                if (otherIndex == uint(-1) || otherIndex >= particleCount)
                    continue;
                
                while (otherIndex < particleCount && cellKeyBuffer[otherIndex] == cellKey)
                {
                    float2 otherPos = positionBuffer[otherIndex];
                    float2 offset = particlePos - otherPos;
                    float r = length(offset);
                    if (r < effectRadius)
                        densityBuffer[index] += mass * SmoothingKernal_Poly6(r, effectRadius);
                    otherIndex++;
                }
            }
        }
        else
        {
            for (int i = 0; i < particleCount; i++)
            {
                float2 offset = particlePos - positionBuffer[i];
                float r = length(offset);
                if (r < effectRadius)
                    densityBuffer[index] += mass * SmoothingKernal_Poly6(r, effectRadius);
            };
        }
    }
}