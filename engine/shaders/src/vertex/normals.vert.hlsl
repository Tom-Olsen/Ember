#include "vertexShaderCommon.hlsli"



struct VertexInput
{
    uint instanceID : SV_InstanceID;    // Instance ID: System value => built in variable
    float3 position : POSITION;         // position in local/model sapce
    float3 normal : NORMAL;             // normal in local/model space
    float3 tangent : TANGENT;           // tangent in local/model space
    float4 vertexColor : COLOR;         // vertex color
    float4 uv : TEXCOORD0;
};
struct VertexOutput
{
    float4 clipPosition : SV_POSITION;  // position in clip space: x,y€[-1,1] z€[0,1]
    float3 modelNormal : NORMAL;        // normal in local space
};



VertexOutput main(VertexInput input)
{
    float4 pos = float4(input.position, 1.0f);
    float4x4 localToWorldMatrix = GetLocalToWorldMatrix(input.instanceID);
    float4x4 localToClipMatrix = GetLocalToClipMatrix(input.instanceID, localToWorldMatrix);
    
    VertexOutput output;
    output.clipPosition = mul(localToClipMatrix, pos);
    output.modelNormal = input.normal;
    return output;
}