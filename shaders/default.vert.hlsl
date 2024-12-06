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
    float3 position : POSITION;
    float3 normal : NORMAL;
    float3 tangent : TANGENT;
    float4 color : COLOR;
    float4 uv : TEXCOORD0;
};

struct VertexOutput
{
    float4 position : SV_POSITION;
    float3 normal : NORMAL;
    float3 tangent : TANGENT;
    float3 bitangent : BITANGENT;
    float4 vertexColor : COLOR;
    float4 uv : TEXCOORD0;
    float3 worldPos : TEXCOORD1;
};



VertexOutput main(VertexInput input)
{
    float4 pos = float4(input.position, 1.0f);
    float4 normal = float4(input.normal, 0.0f);
    float4 tangent = float4(input.tangent, 0.0f);
    
    VertexOutput output;
    output.position = mul(localToClipMatrix, pos);
    output.normal = mul(normalMatrix, normal).xyz;
    output.tangent = mul(normalMatrix, tangent).xyz;
    output.bitangent = cross(output.normal, output.tangent);
    output.vertexColor = input.color;
    output.uv = input.uv;
    output.worldPos = mul(modelMatrix, pos).xyz;
    return output;
}