#include "vertexShaderCommon.hlsli"



cbuffer SelectionState : register(b301, CALL_SET)
{
    int selection_state;          // 0 vertexColor, 1 hoverColor, 2 activeColor.
    float4 selection_stateColor;  // only vertices that have this color are effected by state.
    float4 selection_hoverColor;
    float4 selection_activeColor;
};



struct VertexInput
{
    uint instanceID : SV_InstanceID;    // Instance ID: System value => built in variable
    float3 position : POSITION;         // position in local/model sapce
    float3 normal : NORMAL;             // normal in local/model space
    float4 vertexColor : COLOR;         // vertex color
};
struct VertexOutput
{
    float4 clipPosition : SV_POSITION;  // position in clip space: x,y in [-1,1] z in [0,1]
    float3 localNormal : NORMAL;        // normal in local space
    float4 vertexColor : COLOR;         // vertex color
};



VertexOutput main(VertexInput input)
{
    float4 pos = float4(input.position, 1.0f);
    float4x4 localToWorldMatrix = Model_GetLocalToWorldMatrix(input.instanceID);
    float4x4 localToClipMatrix = Model_GetLocalToClipMatrix(input.instanceID, localToWorldMatrix);
    
    VertexOutput output;
    output.clipPosition = mul(localToClipMatrix, pos);
    output.localNormal = input.normal;
    if (selection_state == 1 && all(input.vertexColor == selection_stateColor))
        output.vertexColor = selection_hoverColor;
    else if (selection_state == 2 && all(input.vertexColor == selection_stateColor))
        output.vertexColor = selection_activeColor;
    else
        output.vertexColor = input.vertexColor;
    return output;
}