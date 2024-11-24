SamplerState colorSampler : register(s10);
SamplerComparisonState shadowSampler : register(s11);
Texture2DArray<float> colorTextures : register(t24);
Texture2DArray<float> shadowMaps : register(t21);
#include "pushConstant.hlsli"



struct FragmentInput
{
    float4 position : SV_POSITION;
    float3 normal : NORMAL;
    float4 vertexColor : COLOR;
    float4 uv : TEXCOORD0;
    float3 worldPos : TEXCOORD1;
};


struct Second
{
    //float a[2];
    float a;
};
struct First
{
    Second secondA;
    Second secondB;
};

cbuffer TestBuffer : register(b123)
{
    First firstA;
    //float array[2];
    First firstB[2];
}



float4 main(FragmentInput input) : SV_TARGET
{
    // Mesh data:
    float2 uv = input.uv.xy;
    float3 normal = normalize(input.normal);
    float3 worldPos = input.worldPos;
    
    // Shading:
    float4 color = colorTextures.Sample(colorSampler, float3(uv, 0));
    color.a *= firstA.secondA.a;
   
    return color;
}