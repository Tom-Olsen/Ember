#include "vertexShaderCommon.hlsli"
#include "sceneSet.hlsli"



struct VertexInput
{
    uint instanceID : SV_InstanceID; // Instance ID: System value => built in variable
    float3 position : POSITION;
};
struct VertexOutput
{
    float4 position : SV_POSITION;
    uint layer : SV_RenderTargetArrayIndex;
};



VertexOutput main(VertexInput input)
{
    float4 pos = float4(input.position, 1.0);
    float4x4 localToWorldMatrix = GetLocalToWorldMatrix(input.instanceID);
    float4x4 worldToClipMatrix = GetShadowWorldToClipMatrix(pc.targetIndex);
    float4x4 localToClipMatrix = mul(worldToClipMatrix, localToWorldMatrix);
    
    VertexOutput output;
    output.position = mul(localToClipMatrix, pos);
    output.layer = pc.targetIndex;
    return output;
}