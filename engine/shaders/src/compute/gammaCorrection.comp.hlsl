#define EMBER_SCENE_COLOR_ACCESS EMBER_SCENE_COLOR_ACCESS_IN_PLACE
#include "computeShaderCommon.hlsli"



float3 ToneMapReinhard(float3 linearColor)
{
    // Compress each HDR channel into display range without clipping highlights at 1.
    float3 nonNegativeColor = max(linearColor, 0.0f);
    return nonNegativeColor / (1.0f + nonNegativeColor);
}
float3 ToneMapAcesFitted(float3 linearColor)
{
    // Narkowicz's fitted ACES filmic curve, applied in linear RGB before sRGB conversion.
    // https://knarkowicz.wordpress.com/2016/01/06/aces-filmic-tone-mapping-curve/
    float3 nonNegativeColor = max(linearColor, 0.0f);
    float3 numerator = nonNegativeColor * (2.51f * nonNegativeColor + 0.03f);
    float3 denominator = nonNegativeColor * (2.43f * nonNegativeColor + 0.59f) + 0.14f;
    return saturate(numerator / denominator);
}
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
        float4 color = Scene_GetColor(threadID.xy);
        float3 correctedRGB = LinearToSRGB(ToneMapAcesFitted(color.rgb));
        //float3 correctedRGB = LinearToSRGB(ToneMapReinhard(color.rgb));
        Scene_SetColor(threadID.xy, float4(correctedRGB, color.a));
    }
}