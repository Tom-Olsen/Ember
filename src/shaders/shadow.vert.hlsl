#include "shadowPushConstant.hlsli"



struct VertexInput
{
    float3 position : POSITION;
};

struct VertexOutput
{
    float4 position : SV_POSITION;
    uint layer : SV_RenderTargetArrayIndex;
};



VertexOutput main(VertexInput input)
{
    float4 pos = float4(input.position, 1.0);
    
    VertexOutput output;
    output.position = mul(pc.localToClipMatrix, pos);
    output.layer = pc.shadowMapIndex;
    return output;
}