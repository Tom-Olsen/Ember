#include "computeShaderCommon.hlsli"



cbuffer OutlineProperties : register(b300, SHADER_SET)
{
    int outlineRadius;
};
[[vk::image_format("r8")]] RWTexture2D<float> inputMask : register(u200, CALL_SET);
[[vk::image_format("r8")]] RWTexture2D<float> outputMask : register(u201, CALL_SET);



[numthreads(32, 32, 1)]
void main(uint3 threadID : SV_DispatchThreadID)
{
    if (threadID.x >= pc.threadCount.x || threadID.y >= pc.threadCount.y)
        return;

    int maxX = int(pc.threadCount.x) - 1;
    float expandedMask = 0.0f;
    for (int x = -outlineRadius; x <= outlineRadius; x++)
    {
        int2 srcPixel = int2(clamp(int(threadID.x) + x, 0, maxX), threadID.y);
        expandedMask = max(expandedMask, inputMask[srcPixel]);
    }

    outputMask[threadID.xy] = expandedMask;
}