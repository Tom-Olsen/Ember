#include "linearAlgebra.hlsli"
#include "shadingPushConstant.hlsli"



cbuffer RenderMatrizes : register(b0)
{
    float4x4 modelMatrix;       // mesh local to world matrix
    float4x4 viewMatrix;        // camera world to local matrix
    float4x4 projMatrix;        // camera projection matrix
    float4x4 normalMatrix;      // rotation matrix for normals and directions: (model^-1)^T
    float4x4 localToClipMatrix; // local to camera clip space matrix: (projection * view * model)
};



struct VertexInput
{
    float3 position : POSITION; // position in local/model sapce
    float3 normal : NORMAL;     // normal in local/model space
    float3 tangent : TANGENT;   // tangent in local/model space
    float4 color : COLOR;
    float4 uv : TEXCOORD0;
};

struct VertexOutput
{
    float4 clipPosition : SV_POSITION;  // position in clip space: x,y€[-1,1] z€[0,1]
};



VertexOutput main(VertexInput input)
{ 
    VertexOutput output;
    output.clipPosition = mul(localToClipMatrix, float4(input.position, 1.0f));
    return output;
}