#include "defaultPushConstant.hlsli"



cbuffer RenderMatrizes : register(b0)
{
    float4x4 cb_localToWorldMatrix;    // local to world matrix (also known as model matrix).
    float4x4 cb_viewMatrix;            // world to camera matrix.
    float4x4 cb_projMatrix;            // camera projection matrix (HDC => NDC after w division, which happens automatically).
    float4x4 cb_worldToClipMatrix;     // world to camera clip space matrix: (projection * view)
    float4x4 cb_localToClipMatrix;     // local to camera clip space matrix: (projection * view * localToWorldMatrix)
};



struct VertexInput
{
    float3 position : POSITION; // position in local/model sapce
};

struct VertexOutput
{
    float4 clipPosition : SV_POSITION;  // position in clip space: x,y�[-1,1] z�[0,1]
};



VertexOutput main(VertexInput input)
{ 
    VertexOutput output;
    output.clipPosition = mul(cb_localToClipMatrix, float4(input.position, 1.0f));
    return output;
}