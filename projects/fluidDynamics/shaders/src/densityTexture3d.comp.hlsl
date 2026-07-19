#include "computeShaderCommon.hlsli"
#include "hashGridFunctions3d.hlsli"
#include "smoothingKernals3d.hlsli"



cbuffer Values : register(b300, SHADER_SET)
{
    uint hashGridSize;
    float effectRadius;
    float mass;
};
cbuffer CallValues : register(b301, CALL_SET)
{
    uint particleCount;
    float3 boundsMin;
    float3 boundsMax;
    float4x4 boundsRotation;
};
StructuredBuffer<uint> cellKeyBuffer : register(t100, CALL_SET);
StructuredBuffer<uint> startIndexBuffer : register(t101, CALL_SET);
StructuredBuffer<float3> positionBuffer : register(t102, CALL_SET);
[[vk::image_format("r32f")]] RWTexture3D<float> densityTexture : register(u200, CALL_SET);



[numthreads(8, 8, 4)]
void main(uint3 threadID : SV_DispatchThreadID)
{
    if (threadID.x < pc.threadCount.x && threadID.y < pc.threadCount.y && threadID.z < pc.threadCount.z)
    {
        // Bounds geometry:
        float3 voxelSize = (boundsMax - boundsMin) / float3(pc.threadCount);
        float3 boundsCenter = 0.5f * (boundsMin + boundsMax);

        // Position in bounds-local/simulation space:
        float3 positionFluid = boundsMin + (float3(threadID) + 0.5f) * voxelSize;
        float3 positionSimulation = boundsCenter + mul(boundsRotation, float4(positionFluid - boundsCenter, 0.0f)).xyz;

        float density = 0.0f;
        int3 voxelCell = HashGrid3d_Cell(positionSimulation, effectRadius);
        for (uint i = 0; i < 27; i++)
        {
            int3 neighbourCell = voxelCell + offsets[i];
            uint cellKey = HashGrid3d_GetCellKey(neighbourCell, hashGridSize);
            uint otherIndex = HashGrid3d_GetStartIndex(neighbourCell, hashGridSize, startIndexBuffer);

            // Skip empty cells:
            if (otherIndex == uint(-1) || otherIndex >= particleCount)
                continue;

            while (otherIndex < particleCount && cellKeyBuffer[otherIndex] == cellKey)
            {
                float3 offset = positionSimulation - positionBuffer[otherIndex];
                float r = length(offset);
                if (r < effectRadius)
                    density += mass * SmoothingKernal_Poly6(r, effectRadius);
                otherIndex++;
            }
        }
        densityTexture[threadID] = density;
    }
}