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
    float3 position : POSITION; // position in local/model sapce
    float3 normal : NORMAL; // normal in local/model space
    float3 tangent : TANGENT; // tangent in local/model space
    float4 vertexColor : COLOR; // vertex color
    float4 uv : TEXCOORD0;
};

struct VertexOutput
{
    float4 clipPosition : SV_POSITION; // position in clip space: x,y€[-1,1] z€[0,1]
    float3 worldNormal : NORMAL; // normal in world space
    float3 worldTangent : TANGENT; // tangent in world space
    float4 vertexColor : COLOR; // vertex color
    float4 uv : TEXCOORD0; // texture coordinates
    float3 worldPosition : TEXCOORD1; // position in world space
};



VertexOutput main(VertexInput input)
{
    float4 pos = float4(input.position, 1.0f);
    float4 normal = float4(input.normal, 0.0f);
    float4 tangent = float4(input.tangent, 0.0f);
    float4x4 localToWorldMatrix = cb_localToWorldMatrix;
    if (pc.instanceCount != 0 && input.instanceID < pc.instanceCount)
        localToWorldMatrix = mul(cb_localToWorldMatrix, instanceBuffer[input.instanceID].localToWorldMatrix);
    float4x4 localToClipMatrix = mul(cb_worldToClipMatrix, localToWorldMatrix);
    float4x4 normalMatrix = LinAlg_NormalMatrix(localToWorldMatrix);
    
    VertexOutput output;
    output.clipPosition = mul(localToClipMatrix, pos);
    output.worldNormal = mul(normalMatrix, normal).xyz;
    output.worldTangent = mul(normalMatrix, tangent).xyz;
    output.vertexColor = input.vertexColor;
    if (pc.instanceCount != 0 && input.instanceID < pc.instanceCount)
        output.vertexColor *= instanceBuffer[input.instanceID].color;
    output.uv = input.uv;
    output.worldPosition = mul(localToWorldMatrix, pos).xyz;
    return output;
}