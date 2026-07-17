#include "computeShaderCommon.hlsli"



[[vk::image_format("rgba16f")]] RWTexture2D<float4> inputImage : register(u200, CALL_SET);
[[vk::image_format("rgba16f")]] RWTexture2D<float4> outputImage : register(u201, CALL_SET);



float3 LinearToSRGB(float3 linearColor)
{
    float3 sRGBLo = 12.92f * linearColor;
    float3 sRGBHi = 1.055f * pow(linearColor, 1.0f / 2.4f) - 0.055f;

    float3 result;
    result.r = (linearColor.r <= 0.0031308f) ? sRGBLo.r : sRGBHi.r;
    result.g = (linearColor.g <= 0.0031308f) ? sRGBLo.g : sRGBHi.g;
    result.b = (linearColor.b <= 0.0031308f) ? sRGBLo.b : sRGBHi.b;

    return result;
}



[numthreads(32, 32, 1)]
void main(uint3 threadID : SV_DispatchThreadID)
{
    if (threadID.x < pc.threadCount.x && threadID.y < pc.threadCount.y)
    {
        float4 color = inputImage[threadID.xy];
        float3 correctedRGB = LinearToSRGB(clamp(color.rgb, 0, 1));
        outputImage[threadID.xy] = float4(correctedRGB, color.a);
    }
}