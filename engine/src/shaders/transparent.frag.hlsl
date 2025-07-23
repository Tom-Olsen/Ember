SamplerState colorSampler : register(s10);
SamplerComparisonState shadowSampler : register(s11);
Texture2DArray<float> shadowMaps : register(t20);
Texture2D colorMap : register(t21); // format = VK_FORMAT_R8G8B8A8_SRGB,
Texture2D normalMap : register(t22); // format = VK_FORMAT_R8G8B8A8_UNORM,   opengl style unorm normal map
Texture2D metallicityMap : register(t23); // format = VK_FORMAT_R8_UNORM,         single channel unorm metallicity map
Texture2D roughnessMap : register(t24); // format = VK_FORMAT_R8_UNORM,         single channel unorm roughness map
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
    float3 worldTangent : TANGENT;      // tangent in world space
    float4 vertexColor : COLOR;         // vertex color
    float4 uv : TEXCOORD0;              // texture coordinates
    float3 worldPosition : TEXCOORD1;   // position in world space
};



float4 main(FragmentInput input) : SV_TARGET
{
    // Mesh data:
    float2 uv = input.uv.xy * scaleOffset.xy + scaleOffset.zw;
    float4 col = input.vertexColor;
    
    // Shading:
    float4 color = col * diffuseColor * colorMap.Sample(colorSampler, uv);
    
    return color;
}