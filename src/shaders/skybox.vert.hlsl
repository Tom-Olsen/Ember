#include "shadingPushConstant.hlsli"
#include "mathf.hlsli"



cbuffer RenderMatrizes : register(b0)
{
    float4x4 modelMatrix;       // mesh local to world matrix
    float4x4 viewMatrix;        // camera world to local matrix
    float4x4 projMatrix;        // camera projection matrix
    float4x4 normalMatrix;      // rotation matrix for directions: (model^-1)^T
    float4x4 localToClipMatrix; // local to camera clip space matrix: (projection * view * model)
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
    float4x4 fakeView = viewMatrix;
    fakeView[0][3] = fakeView[1][3] = fakeView[2][3] = 0.0f;
    float4x4 mat = mul(projMatrix, fakeView);
    
    VertexOutput output;
    output.clipPosition = mul(mat, pos);
    output.localPos = mul(LinAlg_RotateX3x3(-mathf_PI_2), input.position);
    return output;
}