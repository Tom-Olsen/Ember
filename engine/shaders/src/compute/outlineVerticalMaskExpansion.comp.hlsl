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

    int maxX = int(pc.threadCount.x) - 1;
    int maxY = int(pc.threadCount.y) - 1;
    float expandedMask = 0.0f;
    for (int y = -outlineRadius; y <= outlineRadius; y++)
    {
        int2 srcPixel = int2(threadID.x, clamp(int(threadID.y) + y, 0, maxY));
        expandedMask = max(expandedMask, inputMask[srcPixel]);
    }

	// This creates an outline along the screen edge if the selected object crosses it.
    bool isInScreenEdgeBuffer =
        int(threadID.x) < outlineRadius || int(threadID.x) > maxX - outlineRadius ||
        int(threadID.y) < outlineRadius || int(threadID.y) > maxY - outlineRadius;
    float originalMaskValue = isInScreenEdgeBuffer ? 0.0f : originalMask[threadID.xy];
    outputMask[threadID.xy] = expandedMask * (1.0f - originalMaskValue);
}