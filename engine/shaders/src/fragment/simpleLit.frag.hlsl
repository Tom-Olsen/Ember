SamplerComparisonState shadowSampler : register(s11);
Texture2DArray<float> shadowMaps : register(t20);
#include "shadowMapping.hlsli"
#include "defaultPushConstant.hlsli"



cbuffer SurfaceProperties : register(b2)
{
    float4 diffuseColor;    // (1.0, 1.0, 1.0)
    float roughness;        // 0.5
    float3 reflectivity;    // 0.4
    float metallicity;      // 0 = dielectric, 1 = metal
    float4 scaleOffset;     // .xy = scale, .zw = offset
};



struct FragmentInput
{
    float4 clipPosition : SV_POSITION;  // position in clip space: x,y€[-1,1] z€[0,1]
    float3 worldNormal : NORMAL;        // normal in world space
    float3 worldPosition : TEXCOORD1;   // position in world space
};



float4 main(FragmentInput input) : SV_TARGET
{
    // Mesh data:
    float3 worldPos = input.worldPosition;
    float3 worldNormal = normalize(input.worldNormal);
    
    // Lighting:
    float ambient = 0.1f;
    float3 finalColor = ambient * diffuseColor.xyz;
    finalColor += PhysicalLighting(worldPos, pc.cameraPosition.xyz, worldNormal, diffuseColor.xyz, roughness, reflectivity, metallicity, pc.dLightsCount, pc.sLightsCount, directionalLightData, positionalLightData, shadowMaps, shadowSampler);
    
    return float4(finalColor, 1.0f);
}