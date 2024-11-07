#include "linearAlgebra.hlsli"



cbuffer RenderMatrizes : register(b0)
{
    float4x4 modelMatrix;       // mesh local to world matrix
    float4x4 viewMatrix;        // camera world to local matrix
    float4x4 projMatrix;        // camera projection matrix
    float4x4 normalMatrix;      // rotation matrix for normals and directions
    float4x4 localToClipMatrix; // local to clip space matrix: (model * view * projection)
};



struct PushConstant
{
    float4 time;
    float4 delaTime;
};
#if defined(_DXC)
[[vk::push_constant]] CullPushConstants pc;
#else
[[vk::push_constant]] ConstantBuffer<PushConstant> pc;
#endif



struct VertexInput
{
    float3 position : POSITION;
    float3 normal : NORMAL;
    float4 color : COLOR;
    float4 uv : TEXCOORD0;
};

struct VertexOutput
{
    float4 position : SV_POSITION;
    float3 normal : NORMAL;
    float4 vertexColor : COLOR;
};



VertexOutput main(VertexInput input)
{
    float4 pos = float4(input.position, 1.0);
    float3 normal = normalize(mul(normalMatrix, float4(input.normal, 0.0f)).xyz);
    
    VertexOutput output;
    output.position = mul(localToClipMatrix, pos);
    output.normal = normal;
    output.vertexColor = input.color;
    return output;
}

// Notes:
// - SV = System Value
// - SV_POSITION is a system value that represents the position of a vertex in clip space