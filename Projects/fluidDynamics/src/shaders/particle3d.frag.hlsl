SamplerComparisonState shadowSampler : register(s11);
Texture2DArray<float> shadowMaps : register(t20);
#include "shadowMapping.hlsli"
#include "defaultPushConstant.hlsli"



cbuffer SurfaceProperties : register(b7)
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
};



float4 main(FragmentInput input) : SV_TARGET
{
    float2 uv = input.uv.xy;
    float r = length(2 * uv - float2(1, 1));
    float alpha = (1 - r);
    
    float4 color = input.vertexColor * diffuseColor;
    color.a = alpha;
    
    return color;
}