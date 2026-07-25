#include "vertexShaderCommon.hlsli"



struct VertexInput
{
    float3 positionLocal : POSITION;    // position in local/model sapce
};
struct VertexOutput
{
    float4 positionClip : SV_POSITION;  // position in clip space: x,y in [-1,1] z in[0,1]
    float3 positionWorld : TEXCOORD0;   // position in world space
    float3 positionLocal : TEXCOORD1;   // position in local/model space
};



VertexOutput main(VertexInput input)
{
    float4x4 localToWorldMatrix = GetLocalToWorldMatrix(0);
    float4x4 localToClipMatrix = GetLocalToClipMatrix(0, localToWorldMatrix);

    VertexOutput output;
    output.positionClip = mul(localToClipMatrix, float4(input.positionLocal, 1.0f));
    output.positionWorld = mul(localToWorldMatrix, float4(input.positionLocal, 1.0f)).xyz;
    output.positionLocal = input.positionLocal;
    return output;
}