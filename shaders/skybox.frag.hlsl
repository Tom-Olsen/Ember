SamplerState colorSampler : register(s10);
TextureCube colorMap : register(t20);
#include "shadingPushConstant.hlsli"



struct FragmentInput
{
    float4 position : SV_POSITION;  // position in clip space: x,y€[-1,1] z€[0,1]
    float3 localPos : TEXCOORD1;    // position in local space
};



struct FragmentOutput
{
    float4 color : SV_Target;
    float depth : SV_Depth;
};



FragmentOutput main(FragmentInput input)
{
    // Mesh data:
    float3 direction = normalize(input.localPos.xyz);
    
    FragmentOutput output;
    output.color = colorMap.Sample(colorSampler, direction);
    output.depth = 1.0f;
    return output;
}