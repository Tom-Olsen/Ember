#include "fragmentShaderCommon.hlsli"



struct FragmentInput
{
    float4 clipPosition : SV_POSITION;  // position in clip space: x,y€[-1,1] z€[0,1]
    float3 modelNormal : NORMAL;        // normal in local space
};



float4 main(FragmentInput input) : SV_TARGET
{
    return float4(input.modelNormal, 1.0f);
}