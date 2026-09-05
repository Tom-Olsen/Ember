#include "computeShaderCommon.hlsli"



cbuffer OutlineProperties : register(b300, SHADER_SET)
{
    float4 outlineColor;
};
[[vk::image_format("r8")]] RWTexture2D<float> outlineMask : register(u200, CALL_SET);
[[vk::image_format("rgba16f")]] RWTexture2D<float4> inOutImage : register(u201, CALL_SET);



[numthreads(32, 32, 1)]
void main(uint3 threadID : SV_DispatchThreadID)
{
    if (threadID.x >= pc.threadCount.x || threadID.y >= pc.threadCount.y)
        return;

    float4 renderColor = inOutImage[threadID.xy];
    renderColor.rgb = lerp(renderColor.rgb, outlineColor.rgb, outlineMask[threadID.xy] * outlineColor.a);
    inOutImage[threadID.xy] = renderColor;
}