SamplerState colorSampler : register(s10);
SamplerComparisonState shadowSampler : register(s11);
Texture2D colorTexture : register(t20);
Texture2DArray<float> shadowMaps : register(t21);
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
    float4 vertexColor : COLOR;
    float4 uv : TEXCOORD0;
    float3 worldPos : TEXCOORD1;
};



float4 main(FragmentInput input) : SV_TARGET
{
    // Mesh data:
    float2 uv = input.uv.xy;
    float3 normal = normalize(input.normal);
    float3 worldPos = input.worldPos;
    
    // Shading:
    float4 color = colorTexture.Sample(colorSampler, uv);
    
    // Lighting:
    float ambient = 0.1f;
    float3 finalColor = ambient * color.xyz;
    finalColor += DirectionalShadows(worldPos, normal, color.xyz, directionalLightData, shadowMaps, shadowSampler);
    finalColor += SpotShadows(worldPos, normal, color.xyz, spotLightData, shadowMaps, shadowSampler);
    
    return float4(finalColor, 1.0f);
}