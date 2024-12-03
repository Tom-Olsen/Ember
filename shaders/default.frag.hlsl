SamplerState colorSampler : register(s10);
SamplerComparisonState shadowSampler : register(s11);
Texture2DArray<float> shadowMaps : register(t20);
Texture2D colorTexture : register(t21);
Texture2D roughnessMap : register(t22);
Texture2D normalMap : register(t23);
#include "shadowMapping.hlsli"
#include "shadingPushConstant.hlsli"



cbuffer SurfaceProperties : register(b1)
{
    float4 diffuseColor;    // (1.0, 1.0, 1.0)
    float roughness;        // 0.5
    float3 reflectivity;    // 0.4
    bool metallic;
    float4 scaleOffset;     // .xy = scale, .zw = offset
};



struct FragmentInput
{
    float4 position : SV_POSITION;
    float3 normal : NORMAL;
    float3 tangent : TANGENT;
    float3 binormal : BINORMAL;
    float4 vertexColor : COLOR;
    float4 uv : TEXCOORD0;
    float3 worldPos : TEXCOORD1;
};



float4 main(FragmentInput input) : SV_TARGET
{
    // Mesh data:
    float2 uv = input.uv.xy * scaleOffset.xy + scaleOffset.zw;
    float3 normal = normalize(input.normal);
    float3 tangent = normalize(input.tangent);
    float3 binormal = normalize(input.binormal);
    float3x3 TBN = float3x3(tangent, binormal, normal);
    float3 worldPos = input.worldPos;
    
    // Shading:
    float3 tangentNormal = normalize(normalMap.Sample(colorSampler, uv).xyz * 2.0 - 1.0);   
    float3 worldNormal = normalize(mul(TBN, tangentNormal));
    float4 color = diffuseColor * colorTexture.Sample(colorSampler, uv);
    float finalRoughness = roughness * roughnessMap.Sample(colorSampler, uv).r;
    
    // Lighting:
    float ambient = 0.1f;
    float3 finalColor = ambient * color.xyz;
    finalColor += PhysicalLighting(worldPos, pc.cameraPosition.xyz, normal, color.xyz, finalRoughness, reflectivity, metallic, pc.dLightsCount, pc.sLightsCount, pc.pLightsCount, directionalLightData, spotLightData, pointLightData, shadowMaps, shadowSampler);
    
    //return float4(normal.xyz, 1.0f);
    //return float4(-binormal.xyz, 1.0f);
    //return float4(tangent.xyz, 1.0f);
    //return float4(mul(TBN, float3(0, 0, 1)), 1);
    //return float4(worldNormal.xyz, 1.0f);
    return float4(finalColor, 1.0f);
}