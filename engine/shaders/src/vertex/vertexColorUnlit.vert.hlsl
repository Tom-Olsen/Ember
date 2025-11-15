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
    float4x4 cb_viewMatrix;         // world to camera matrix.
    float4x4 cb_projMatrix;         // camera projection matrix (HDC => NDC after w division, which happens automatically).
    float4x4 cb_worldToClipMatrix;  // world to camera clip space matrix: (projection * view)
    float4x4 cb_localToClipMatrix;  // local to camera clip space matrix: (projection * view * localToWorldMatrix)
};



struct VertexInput
{
    uint instanceID : SV_InstanceID;    // Instance ID: System value => built in variable
    float3 position : POSITION;         // position in local/model sapce
    float4 vertexColor : COLOR;         // vertex color
};

struct VertexOutput
{
    float4 clipPosition : SV_POSITION;  // position in clip space: x,y€[-1,1] z€[0,1]
    float4 vertexColor : COLOR;         // vertex color
};



VertexOutput main(VertexInput input)
{
    float4 pos = float4(input.position, 1.0);
    float4x4 localToWorldMatrix = cb_localToWorldMatrix;
    if (pc.instanceCount != 0 && input.instanceID < pc.instanceCount)
        localToWorldMatrix = mul(cb_localToWorldMatrix, instanceBuffer[input.instanceID].localToWorldMatrix);
    float4x4 localToClipMatrix = mul(cb_worldToClipMatrix, localToWorldMatrix);
    
    VertexOutput output;
    output.clipPosition = mul(localToClipMatrix, pos);
    output.vertexColor = input.vertexColor;
    return output;
}