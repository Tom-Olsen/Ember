SamplerState colorSampler : register(s10);
SamplerComparisonState shadowSampler : register(s11);
Texture2D colorTexture : register(t20);
Texture2DArray<float> shadowMaps : register(t21);
#include "shadowMapping.hlsli"
#include "pushConstant.hlsli"



cbuffer SurfaceProperties : register(b1)
{
    float4 diffuseColor;    // (1.0, 1.0, 1.0)
    float roughness;        // 0.5
    float3 reflectivity;    // 0.4
    bool metallic;
};



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
    float4 color = diffuseColor * colorTexture.Sample(colorSampler, uv);
    
    // Lighting:
    float ambient = 0.1f;
    float3 finalColor = ambient * color.xyz;
    finalColor += PhysicalLighting(worldPos, pc.cameraPosition.xyz, normal, color.xyz, roughness, reflectivity, metallic, pc.dLightsCount, pc.sLightsCount, directionalLightData, spotLightData, shadowMaps, shadowSampler);
    
    return float4(finalColor, 1.0f);
}