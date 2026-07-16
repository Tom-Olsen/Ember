#include "vertexShaderCommon.hlsli"



struct VertexInput
{
    uint instanceID : SV_InstanceID;
    float3 position : POSITION;
};
struct VertexOutput
{
    float4 clipPosition : SV_POSITION;
};



VertexOutput main(VertexInput input)
{
    float4x4 localToWorldMatrix = GetLocalToWorldMatrix(input.instanceID);
    float4x4 localToClipMatrix = GetLocalToClipMatrix(input.instanceID, localToWorldMatrix);

    VertexOutput output;
    output.clipPosition = mul(localToClipMatrix, float4(input.position, 1.0f));
    return output;
}