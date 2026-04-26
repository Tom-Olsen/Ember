#include "fragmentShaderCommon.hlsli"



cbuffer SurfaceProperties : register(b300, CALL_SET)
{
    float4 diffuseColor;    // (1.0, 1.0, 1.0)
};



struct FragmentInput
{
    float4 clipPosition : SV_POSITION;  // position in clip space: x,y€[-1,1] z€[0,1]
};



float4 main(FragmentInput input) : SV_TARGET
{
    return diffuseColor;
}