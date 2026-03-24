#include "fragmentShaderCommon.hlsli"
TextureCube colorMap : register(t100, SHADER_SET);



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