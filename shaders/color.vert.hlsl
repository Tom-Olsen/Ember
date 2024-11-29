#include "linearAlgebra.hlsli"
#include "shadingPushConstant.hlsli"



cbuffer RenderMatrizes : register(b0)
{
    float4x4 modelMatrix;       // mesh local to world matrix
    float4x4 viewMatrix;        // camera world to local matrix
    float4x4 projMatrix;        // camera projection matrix
    float4x4 normalMatrix;      // rotation matrix for normals and directions
    float4x4 localToClipMatrix; // local to clip space matrix: (model * view * projection)
};



struct VertexInput
{
    float3 position : POSITION;
    float3 normal : NORMAL;
    float4 color : COLOR;
};

struct VertexOutput
{
    float4 position : SV_POSITION;
    float3 normal : NORMAL;
    float4 vertexColor : COLOR;
    float3 worldPos : TEXCOORD0;
};



VertexOutput main(VertexInput input)
{
    float4 pos = float4(input.position, 1.0);
    float4 normal = float4(input.normal, 0.0);
    
    VertexOutput output;
    output.position = mul(localToClipMatrix, pos);
    output.normal = mul(normalMatrix, normal).xyz;
    output.vertexColor = input.color;
    output.worldPos = mul(modelMatrix, pos).xyz;
    return output;
}