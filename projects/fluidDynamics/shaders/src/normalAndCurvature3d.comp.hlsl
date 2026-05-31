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
StructuredBuffer<float> densityBuffer : register(t103, CALL_SET);
RWStructuredBuffer<float3> normalBuffer : register(u200, CALL_SET);
RWStructuredBuffer<float> curvatureBuffer : register(u201, CALL_SET);



[numthreads(128, 1, 1)]
void main(uint3 threadID : SV_DispatchThreadID)
{
    uint index = threadID.x;
    if (index < pc.threadCount.x)
    {
        normalBuffer[index] = 0;
        curvatureBuffer[index] = 0;
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

                    float3 otherPos = positionBuffer[otherIndex];
                    float3 offset = particlePos - otherPos;
                    float r = length(offset);
                    if (r < effectRadius && r > 1e-8f)
                    {
                        float3 dir = offset / r;
                        float c = mass / densityBuffer[otherIndex];
                        normalBuffer[index] += c * SmoothingKernal_DPoly6(r, dir, effectRadius);
                        curvatureBuffer[index] -= c * SmoothingKernal_DDPoly6(r, effectRadius);
                    }
                    otherIndex++;
                }
            }
            float normalLength = length(normalBuffer[index]);
            if (normalLength > 1e-3f)
                curvatureBuffer[index] /= normalLength;
            else
                curvatureBuffer[index] = 0;
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
                if (r < effectRadius && r > 1e-8f)
                {
                    float3 dir = offset / r;
                    float c = mass / densityBuffer[i];
                    normalBuffer[index] += c * SmoothingKernal_DPoly6(r, dir, effectRadius);
                    curvatureBuffer[index] -= c * SmoothingKernal_DDPoly6(r, effectRadius);
                }
            };
            float normalLength = length(normalBuffer[index]);
            if (normalLength > 1e-3f)
                curvatureBuffer[index] /= normalLength;
            else
                curvatureBuffer[index] = 0;
        }
    }
}