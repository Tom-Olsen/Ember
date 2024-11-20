#include "shadowMapping.hlsli"
cbuffer LightMatrizes : register(b0)
{
    float4x4 localToClipMatrixTest[MAX_D_LIGHTS]; // local to camera clip space matrix: (projection * view * model)
};



struct VertexInput
{
    float3 position : POSITION;
    uint layerIndex : SV_InstanceID; // Instance index used to select the depth layer
};

struct VertexOutput
{
    float4 position : SV_POSITION;
    uint layer : SV_RenderTargetArrayIndex;
};



VertexOutput main(VertexInput input)
{
    float4 pos = float4(input.position, 1.0);
    pos.w = 1.0f;
    
    VertexOutput output;
    output.position = mul(localToClipMatrixTest[input.layerIndex], pos);
    output.layer = input.layerIndex;
    return output;
}