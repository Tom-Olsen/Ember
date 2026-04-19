#include "fragmentShaderCommon.hlsli"



cbuffer SurfaceProperties : register(b2)
{
    float4 diffuseColor;    // (1.0, 1.0, 1.0)
};



struct FragmentInput
{
    float4 clipPosition : SV_POSITION;  // position in clip space: x,y in [-1,1] z in [0,1]
    float4 vertexColor : COLOR;         // vertex color
};



float4 main(FragmentInput input) : SV_TARGET
{
    return diffuseColor * input.vertexColor;
}