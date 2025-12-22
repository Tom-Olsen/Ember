#include "defaultPushConstant.hlsli"
#include "math.hlsli"



struct InstanceData
{
    float4x4 localToWorldMatrix;
    float4 color;
};
StructuredBuffer<InstanceData> instanceBuffer : register(t0);



cbuffer RenderMatrizes : register(b1)
{
    float4x4 cb_localToWorldMatrix; // local to world matrix (also known as model matrix).
    float4x4 cb_viewMatrix; // world to camera matrix.
    float4x4 cb_projMatrix; // camera projection matrix (HDC => NDC after w division, which happens automatically).
    float4x4 cb_worldToClipMatrix; // world to camera clip space matrix: (projection * view)
    float4x4 cb_localToClipMatrix; // local to camera clip space matrix: (projection * view * localToWorldMatrix)
};

struct VertexInput
{
    uint instanceID : SV_InstanceID; // Instance ID: System value => built in variable
    float4 position : POSITION; // position in local/model sapce
    float3 normal : NORMAL; // normal in local/model space
    float3 tangent : TANGENT; // tangent in local/model space
    float4 vertexColor : COLOR; // vertex color
    float4 uv : TEXCOORD0;
    float b[2] : TEXCOORD01;
};

struct VertexOutput
{
    float4 position : SV_POSITION; // position in local/model sapce
    float3 normal : NORMAL; // normal in local/model space
    float3 tangent : TANGENT; // tangent in local/model space
    float4 vertexColor : COLOR; // vertex color
    float4 uv : TEXCOORD0;
    float b[2] : TEXCOORD01;
};



VertexOutput main(VertexInput input)
{
    VertexOutput output;
    output.position = input.position;
    output.normal = input.normal;
    //output.tangent = input.tangent;
    //output.vertexColor = input.vertexColor;
    //output.uv = input.uv;
    output.b = input.b;
    
    if (pc.instanceCount != 0 && input.instanceID < pc.instanceCount)
        output.position *= 15;
    
    return output;
}