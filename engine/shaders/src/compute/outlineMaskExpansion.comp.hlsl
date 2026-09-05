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

    int2 maxCoordinate = int2(pc.threadCount.xy) - 1;
    float expandedMask = 0.0f;
    for (int y = -outlineRadius; y <= outlineRadius; y++)
        for (int x = -outlineRadius; x <= outlineRadius; x++)
        {
            int2 srcPixel = clamp(int2(threadID.xy) + int2(x, y), int2(0, 0), maxCoordinate);
            expandedMask = max(expandedMask, inputMask[srcPixel]);
        }

    outputMask[threadID.xy] = expandedMask * (1.0f - inputMask[threadID.xy]);
}