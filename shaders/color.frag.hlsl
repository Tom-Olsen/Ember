SamplerComparisonState shadowSampler : register(s10);
Texture2DArray<float> shadowMaps : register(t25);
#include "shadowMapping.hlsli"
#include "shadingPushConstant.hlsli"



cbuffer SurfaceProperties : register(b1)
{
    float4 diffuseColor;    // (1.0, 1.0, 1.0)
    float roughness;        // 0.5
    float3 reflectivity;    // 0.4
    bool metallic;
    float4 scaleOffset; // .xy = scale, .zw = offset
};



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
    float4 color = diffuseColor * input.color;
    float3 normal = normalize(input.normal);
    float3 worldPos = input.worldPos;
    
    // Lighting:
    float ambient = 0.1f;
    float3 finalColor = ambient * color.xyz;
    finalColor += PhysicalLighting(worldPos, pc.cameraPosition.xyz, normal, color.xyz, roughness, reflectivity, metallic, pc.dLightsCount, pc.sLightsCount, pc.pLightsCount, directionalLightData, spotLightData, pointLightData, shadowMaps, shadowSampler);
    
    return float4(finalColor, 1.0f);
}