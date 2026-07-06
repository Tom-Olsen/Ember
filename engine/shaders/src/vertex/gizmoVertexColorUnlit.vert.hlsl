#include "vertexShaderCommon.hlsli"



cbuffer SelectionState : register(b301, CALL_SET)
{
    int state;          // 0 vertexColor, 1 hoverColor, 2 activeColor.
    float4 stateColor;  // only vertices that have this color are effected by state.
    float4 hoverColor;
    float4 activeColor;
};




struct VertexInput
{
    uint instanceID : SV_InstanceID;    // Instance ID: System value => built in variable
    float3 position : POSITION;         // position in local/model sapce
    float4 vertexColor : COLOR;         // vertex color
};
struct VertexOutput
{
    float4 clipPosition : SV_POSITION;  // position in clip space: x,y in [-1,1] z in [0,1]
    float4 vertexColor : COLOR;         // vertex color
};



VertexOutput main(VertexInput input)
{
    float4 pos = float4(input.position, 1.0f);
    float4x4 localToWorldMatrix = GetLocalToWorldMatrix(input.instanceID);
    float4x4 localToClipMatrix = GetLocalToClipMatrix(input.instanceID, localToWorldMatrix);
    
    VertexOutput output;
    output.clipPosition = mul(localToClipMatrix, pos);
    if (state == 1 && all(input.vertexColor == stateColor)) // hovered
        output.vertexColor = hoverColor;
    else if (state == 2 && all(input.vertexColor == stateColor)) // activeColor
        output.vertexColor = activeColor;
    else
        output.vertexColor = input.vertexColor;
    return output;
}