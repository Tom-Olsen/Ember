#include "vertexShaderCommon.hlsli"
#include "sceneSet.hlsli"



cbuffer Values : register(b300, SHADER_SET)
{
    float renderWidth;
};



StructuredBuffer<float3> positionBuffer : register(t100, SHADER_SET);



struct VertexInput
{
    uint instanceID : SV_InstanceID;    // Instance ID: System value => built in variable
    float3 position : POSITION;         // position in local/model space
    float4 uv : TEXCOORD0;              // texture coordinates
};
struct VertexOutput
{
    float4 clipPosition : SV_POSITION;
    uint layer : SV_RenderTargetArrayIndex;
};



VertexOutput main(VertexInput input)
{
    float3 centerLocal = positionBuffer[input.instanceID];
    float3 centerWorld = mul(model_localToWorldMatrix, float4(centerLocal, 1.0f)).xyz;
    float2 vertexOffset = renderWidth * (input.uv.xy - 0.5f);
    float3 vertPosWorld = centerWorld + Camera_GetRight() * vertexOffset.x + Camera_GetUp() * vertexOffset.y;
    float4 pos = float4(vertPosWorld, 1.0f);

    VertexOutput output;
    output.clipPosition = mul(Light_GetShadowWorldToClipMatrix(pc.targetIndex), float4(vertPosWorld, 1.0f));
    output.layer = pc.targetIndex;
    return output;
}