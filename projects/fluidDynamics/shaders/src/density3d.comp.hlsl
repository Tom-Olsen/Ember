#include "computeShaderCommon.hlsli"
#include "hashGridFunctions3d.hlsli"
#include "smoothingKernals3d.hlsli"



cbuffer Values : register(b300, SHADER_SET)
{
    // particleCount = pc.threadCount.x
    uint hashGridSize; // ~2*particleCount
    uint useHashGridOptimization;
    float effectRadius;
    float mass;
};
StructuredBuffer<uint> cellKeyBuffer : register(t100, CALL_SET);
StructuredBuffer<uint> startIndexBuffer : register(t101, CALL_SET);
StructuredBuffer<float3> positionBuffer : register(t102, CALL_SET);
RWStructuredBuffer<float> densityBuffer : register(u200, CALL_SET);



[numthreads(128, 1, 1)]
void main(uint3 threadID : SV_DispatchThreadID)
{
    uint index = threadID.x;
    if (index < pc.threadCount.x)
    {
        densityBuffer[index] = 0;
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
                    float3 offset = particlePos - positionBuffer[otherIndex];
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
                float3 offset = particlePos - positionBuffer[i];
                float r = length(offset);
                if (r < effectRadius)
                    densityBuffer[index] += mass * SmoothingKernal_Poly6(r, effectRadius);
            };
        }
    }
}