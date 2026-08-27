#include "vertexShaderCommon.hlsli"



struct VertexInput
{
    float3 position : POSITION;
    float4 uv : TEXCOORD0;
};
struct VertexOutput
{
    float4 clipPosition : SV_POSITION;
    float4 uv : TEXCOORD0;
};



VertexOutput main(VertexInput input)
{
    VertexOutput output;
    output.clipPosition = float4(input.position, 1.0f);
    output.uv = input.uv;
    return output;
}