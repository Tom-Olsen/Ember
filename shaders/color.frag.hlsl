SamplerComparisonState shadowSampler : register(s10);
Texture2DArray<float> shadowMaps : register(t25);
#include "shadowMapping.hlsli"



struct PushConstant
{
    float time;
    float delaTime;
    int dLightsCount;
    int sLightsCount;
    int pLightsCount;
};
#if defined(_DXC)
[[vk::push_constant]] CullPushConstants pc;
#else
[[vk::push_constant]] ConstantBuffer<PushConstant> pc;
#endif



struct FragmentInput
{
    float4 position : SV_POSITION;
    float3 normal : NORMAL;
    float4 color : COLOR;
    float3 worldPos : TEXCOORD0;
};



float4 main(FragmentInput input) : SV_TARGET
{
    // Mesh data:
    float4 color = input.color;
    float3 normal = normalize(input.normal);
    float3 worldPos = input.worldPos;
    
    // Lighting:
    float ambient = 0.1f;
    float3 finalColor = ambient * color.xyz;
    finalColor += DirectionalShadows(worldPos, normal, color.xyz, pc.dLightsCount, directionalLightData, shadowMaps, shadowSampler);
    finalColor += SpotShadows(worldPos, normal, color.xyz, pc.sLightsCount, spotLightData, shadowMaps, shadowSampler);
    
    return float4(finalColor, 1.0f);
}