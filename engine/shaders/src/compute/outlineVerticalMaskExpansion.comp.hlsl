#include "computeShaderCommon.hlsli"



cbuffer OutlineProperties : register(b300, SHADER_SET)
{
    int outlineRadius;
};
[[vk::image_format("r8")]] RWTexture2D<float> originalMask : register(u200, CALL_SET);
[[vk::image_format("r8")]] RWTexture2D<float> inputMask : register(u201, CALL_SET);
[[vk::image_format("r8")]] RWTexture2D<float> outputMask : register(u202, CALL_SET);



[numthreads(32, 32, 1)]
void main(uint3 threadID : SV_DispatchThreadID)
{
    if (threadID.x >= pc.threadCount.x || threadID.y >= pc.threadCount.y)
        return;

    int maxY = int(pc.threadCount.y) - 1;
    float expandedMask = 0.0f;
    for (int y = -outlineRadius; y <= outlineRadius; y++)
    {
        int2 srcPixel = int2(threadID.x, clamp(int(threadID.y) + y, 0, maxY));
        expandedMask = max(expandedMask, inputMask[srcPixel]);
    }

    outputMask[threadID.xy] = expandedMask * (1.0f - originalMask[threadID.xy]);
}