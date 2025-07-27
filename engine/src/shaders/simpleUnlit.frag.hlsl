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
};



float4 main(FragmentInput input) : SV_TARGET
{
    return diffuseColor;
}