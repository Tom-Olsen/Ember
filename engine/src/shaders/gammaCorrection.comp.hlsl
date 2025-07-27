#include "computePushConstant.hlsli"
#include "math.hlsli"



RWTexture2D<float4> inputImage : register(u0);
RWTexture2D<float4> outputImage : register(u1);



float3 LinearToSRGB(float3 linearColor)
{
    float3 sRGBLo = linearColor * 12.92;
    float3 sRGBHi = 1.055 * pow(linearColor, 1.0 / 2.4) - 0.055;

    float3 result;
    result.r = (linearColor.r <= 0.0031308) ? sRGBLo.r : sRGBHi.r;
    result.g = (linearColor.g <= 0.0031308) ? sRGBLo.g : sRGBHi.g;
    result.b = (linearColor.b <= 0.0031308) ? sRGBLo.b : sRGBHi.b;

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