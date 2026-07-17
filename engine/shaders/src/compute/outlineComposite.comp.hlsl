#include "computeShaderCommon.hlsli"



cbuffer OutlineProperties : register(b300, SHADER_SET)
{
    float4 outlineColor;
    int outlineRadius;
};
[[vk::image_format("rgba16f")]] RWTexture2D<float4> renderImage : register(u200, SHADER_SET);
[[vk::image_format("r8")]] RWTexture2D<float> mask : register(u201, SHADER_SET);



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
            int2 srcPixel = clamp(threadID.xy + int2(x, y), int2(0, 0), maxCoordinate);
            expandedMask = max(expandedMask, mask[srcPixel]);
        }

    float outlineMask = expandedMask * (1.0f - mask[threadID.xy]) * outlineColor.a;
    float4 renderColor = renderImage[threadID.xy];
    renderColor.rgb = lerp(renderColor.rgb, outlineColor.rgb, outlineMask);
    renderImage[threadID.xy] = renderColor;
}