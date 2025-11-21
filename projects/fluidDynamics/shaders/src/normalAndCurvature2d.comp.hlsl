#include "computePushConstant.hlsli"
#include "hashGridFunctions2d.hlsli"
#include "math.hlsli"
#include "smoothingKernals2d.hlsli"



cbuffer Values : register(b0)
{
    // particleCount = pc.threadCount.x
    int hashGridSize; // ~2*particleCount
    int useHashGridOptimization;
    float effectRadius;
    float mass;
};
StructuredBuffer<uint> cellKeyBuffer : register(t1);
StructuredBuffer<uint> startIndexBuffer : register(t2);
StructuredBuffer<float2> positionBuffer : register(t3);
StructuredBuffer<float> densityBuffer : register(t4);
RWStructuredBuffer<float2> normalBuffer : register(u5);
RWStructuredBuffer<float> curvatureBuffer : register(u6);



[numthreads(128, 1, 1)]
void main(uint3 threadID : SV_DispatchThreadID)
{
    uint index = threadID.x;
    if (index < pc.threadCount.x)
    {
        normalBuffer[index] = 0;
        curvatureBuffer[index] = 0;
        float2 particlePos = positionBuffer[index];
        if (useHashGridOptimization)
        {
            int2 particleCell = HashGrid2d_Cell(particlePos, effectRadius);
            for (int i = 0; i < 9; i++)
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
                    float2 offset = particlePos - otherPos;
                    float r = length(offset);
                    if (r < effectRadius)
                    {
                        float2 dir = offset / r;
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
        {
            for (int i = 0; i < pc.threadCount.x; i++)
            {
                // Skip self interaction:
                if (i == index)
                    continue;
                
                float2 offset = particlePos - positionBuffer[i];
                float r = length(offset);
                if (r < effectRadius)
                {
                    float2 dir = offset / r;
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