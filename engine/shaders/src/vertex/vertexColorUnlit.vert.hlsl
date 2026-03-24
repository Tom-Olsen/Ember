#include "vertexShaderCommon.hlsli"



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
    float4x4 localToWorldMatrix = GetLocalToWorldMatrix(input.instanceID);
    float4x4 localToClipMatrix = GetLocalToClipMatrix(input.instanceID, localToWorldMatrix);
    
    VertexOutput output;
    output.clipPosition = mul(localToClipMatrix, pos);
    output.vertexColor = input.vertexColor;
    return output;
}