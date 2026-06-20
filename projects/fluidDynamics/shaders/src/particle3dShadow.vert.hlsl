#include "vertexShaderCommon.hlsli"
#include "sceneSet.hlsli"



StructuredBuffer<float3> positionBuffer : register(t100, SHADER_SET);



struct VertexInput
{
    uint instanceID : SV_InstanceID;    // Instance ID: System value => built in variable
    float3 position : POSITION;         // position in local/model space
};
struct VertexOutput
{
    float4 position : SV_POSITION;
    uint layer : SV_RenderTargetArrayIndex;
};



VertexOutput main(VertexInput input)
{
    float4 pos = float4(input.position, 1.0f);
    float4x4 localToWorldMatrix = model_localToWorldMatrix;
    if (pc.instanceCount != 0 && input.instanceID < pc.instanceCount)
    {
        float4x4 positionMatrix = LinAlg_Translate(positionBuffer[input.instanceID]);
        localToWorldMatrix = mul(model_localToWorldMatrix, positionMatrix);
    }
    float4x4 worldToClipMatrix = GetShadowWorldToClipMatrix(pc.targetIndex);
    float4x4 localToClipMatrix = mul(worldToClipMatrix, localToWorldMatrix);

    VertexOutput output;
    output.position = mul(localToClipMatrix, pos);
    output.layer = pc.targetIndex;
    return output;
}