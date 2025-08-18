SamplerState colorSampler : register(s10);
SamplerComparisonState shadowSampler : register(s11);
Texture2D colorMap : register(t20);
TextureCube cubeMap : register(t21);
Texture2DArray<float> shadowMaps : register(t22);



struct FragmentInput
{
    float4 position : SV_POSITION;
    float4 uv : TEXCOORD0;
};



float4 main(FragmentInput input) : SV_TARGET
{
    float2 uv = input.uv.xy;
    float4 colorMapSample = colorMap.Sample(colorSampler, uv);
    float4 cubeMapSample = cubeMap.Sample(colorSampler, normalize(float3(uv, 1.0f)));
    float4 shadowMapSample = shadowMaps.SampleCmp(shadowSampler, float3(uv, 0), 1);
    return colorMapSample * cubeMapSample * shadowMapSample;
}