#include "defaultPushConstant.hlsli"
#include "mathf.hlsli"



cbuffer RenderMatrizes : register(b0)
{
    float4x4 cb_localToWorldMatrix; // local to world matrix (also known as model matrix).
    float4x4 cb_viewMatrix;         // world to camera matrix.
    float4x4 cb_projMatrix;         // camera projection matrix (HDC => NDC after w division, which happens automatically).
    float4x4 cb_worldToClipMatrix;  // world to camera clip space matrix: (projection * view)
    float4x4 cb_localToClipMatrix;  // local to camera clip space matrix: (projection * view * localToWorldMatrix)
};



struct VertexInput
{
    float3 position : POSITION; // position in local/model sapce
};

struct VertexOutput
{
    float4 clipPosition : SV_POSITION;  // position in clip space: x,y€[-1,1] z€[0,1]
    float3 localPos : TEXCOORD1;        // position in local space
};



VertexOutput main(VertexInput input)
{
    float4 pos = float4(input.position, 1.0);
    float4x4 fakeView = cb_viewMatrix;
    fakeView[0][3] = fakeView[1][3] = fakeView[2][3] = 0.0f;
    float4x4 mat = mul(cb_projMatrix, fakeView);
    
    VertexOutput output;
    output.clipPosition = mul(mat, pos);
    output.localPos = mul(LinAlg_RotateX3x3(-mathf_PI_2), input.position);
    return output;
}