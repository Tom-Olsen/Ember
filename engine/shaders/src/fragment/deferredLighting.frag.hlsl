#include "fragmentShaderCommon.hlsli"
#include "shadowMapping.hlsli"



struct FragmentInput
{
    float4 clipPosition : SV_POSITION;
    float4 uv : TEXCOORD0;
};



float4 main(FragmentInput input) : SV_TARGET
{
    return float4(0.0f, 0.0f, 0.0f, 1.0f);
}