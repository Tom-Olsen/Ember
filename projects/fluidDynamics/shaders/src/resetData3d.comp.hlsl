#include "computeShaderCommon.hlsli"



cbuffer CallValues : register(b300, CALL_SET)
{
    uint hashGridSize; // 2 * particleCount rounded up to next prime.
    float initialDistributionRadius;
};
RWStructuredBuffer<uint> cellKeyBuffer : register(u200, CALL_SET);
RWStructuredBuffer<uint> startIndexBuffer : register(u201, CALL_SET);
RWStructuredBuffer<uint> sortPermutationBuffer : register(u202, CALL_SET);
RWStructuredBuffer<float3> positionBuffer : register(u203, CALL_SET);
RWStructuredBuffer<float3> velocityBuffer : register(u204, CALL_SET);
RWStructuredBuffer<float> densityBuffer : register(u205, CALL_SET);
RWStructuredBuffer<float3> normalBuffer : register(u206, CALL_SET);
RWStructuredBuffer<float> curvatureBuffer : register(u207, CALL_SET);
RWStructuredBuffer<float3> forceDensityBuffer : register(u208, CALL_SET);
RWStructuredBuffer<float> nearDensityBuffer : register(u209, CALL_SET);
RWStructuredBuffer<float3> tempBuffer0 : register(u210, CALL_SET);
RWStructuredBuffer<float3> tempBuffer1 : register(u211, CALL_SET);
RWStructuredBuffer<float3> tempBuffer2 : register(u212, CALL_SET);
RWStructuredBuffer<float3> tempBuffer3 : register(u213, CALL_SET);
RWStructuredBuffer<float3> tempBuffer4 : register(u214, CALL_SET);
RWStructuredBuffer<float3> tempBuffer5 : register(u215, CALL_SET);
RWStructuredBuffer<float3> tempBuffer6 : register(u216, CALL_SET);
RWStructuredBuffer<float3> tempBuffer7 : register(u217, CALL_SET);



uint CubeParticleCount(uint shellRadius)
{
    uint width = 2 * shellRadius + 1;
    return width * width * width;
}
int3 CubeShellPosition(uint index)
{
    uint shellRadius = 0;
    while (CubeParticleCount(shellRadius) <= index)
        shellRadius++;

    if (shellRadius == 0)
        return int3(0, 0, 0);

    uint width = 2 * shellRadius + 1;
    uint localIndex = index - CubeParticleCount(shellRadius - 1);
    int radius = int(shellRadius);

    uint faceParticleCount = width * width;
    if (localIndex < 2 * faceParticleCount)
    {
        int z = (localIndex < faceParticleCount) ? -radius : radius;
        uint faceIndex = localIndex % faceParticleCount;
        return int3(int(faceIndex % width) - radius, int(faceIndex / width) - radius, z);
    }

    localIndex -= 2 * faceParticleCount;
    uint sideParticleCount = (width - 2) * width;
    if (localIndex < 2 * sideParticleCount)
    {
        int y = (localIndex < sideParticleCount) ? -radius : radius;
        uint sideIndex = localIndex % sideParticleCount;
        return int3(int(sideIndex % width) - radius, y, int(sideIndex / width) - radius + 1);
    }

    localIndex -= 2 * sideParticleCount;
    uint edgeWidth = width - 2;
    uint edgeParticleCount = edgeWidth * edgeWidth;
    int x = (localIndex < edgeParticleCount) ? -radius : radius;
    uint edgeIndex = localIndex % edgeParticleCount;
    return int3(x, int(edgeIndex % edgeWidth) - radius + 1, int(edgeIndex / edgeWidth) - radius + 1);
}



[numthreads(128, 1, 1)]
void main(uint3 threadID : SV_DispatchThreadID)
{
    uint index = threadID.x;

    if (index < pc.threadCount.x)
    {
        cellKeyBuffer[index] = -1;
        startIndexBuffer[index] = -1;
        startIndexBuffer[index + pc.threadCount.x] = -1;
        if (index + 2 * pc.threadCount.x < hashGridSize) // startIndexBuffer has size hashGridSize.
            startIndexBuffer[index + 2 * pc.threadCount.x] = -1;
        sortPermutationBuffer[index] = -1;

        uint outerShellRadius = 0;
        while (CubeParticleCount(outerShellRadius) < pc.threadCount.x)
            outerShellRadius++;
        float spacing = (outerShellRadius > 0) ? initialDistributionRadius / outerShellRadius : 0.0f;
        positionBuffer[index] = spacing * float3(CubeShellPosition(index));

        velocityBuffer[index] = float3(0, 0, 0);
        densityBuffer[index] = 0.0f;
        normalBuffer[index] = float3(0, 0, 0);
        curvatureBuffer[index] = 0;
        forceDensityBuffer[index] = float3(0, 0, 0);
        nearDensityBuffer[index] = 0.0f;
        tempBuffer0[index] = float3(0, 0, 0);
        tempBuffer1[index] = float3(0, 0, 0);
        tempBuffer2[index] = float3(0, 0, 0);
        tempBuffer3[index] = float3(0, 0, 0);
        tempBuffer4[index] = float3(0, 0, 0);
        tempBuffer5[index] = float3(0, 0, 0);
        tempBuffer6[index] = float3(0, 0, 0);
        tempBuffer7[index] = float3(0, 0, 0);
    }
}