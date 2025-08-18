#include "shadowPushConstant.hlsli"



struct InstanceData
{
    float4x4 localToWorldMatrix;
    float4 color;
};
StructuredBuffer<InstanceData> instanceBuffer : register(t0);



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
    // Extract data from 
    int instanceCount = int(pc.instanceCountAndShadowMapIndexAndLocalToWorldMatrix._41);
    int shadowMapIndex = int(pc.instanceCountAndShadowMapIndexAndLocalToWorldMatrix._42);
    float4x4 localToWorldMatrix = pc.instanceCountAndShadowMapIndexAndLocalToWorldMatrix;
    localToWorldMatrix._41 = 0;
    localToWorldMatrix._42 = 0;
    
    float4 pos = float4(input.position, 1.0);
    if (instanceCount != 0 && input.instanceID < instanceCount)
        localToWorldMatrix = mul(localToWorldMatrix, instanceBuffer[input.instanceID].localToWorldMatrix);
    float4x4 localToClipMatrix = mul(pc.worldToClipMatrix, localToWorldMatrix);
    
    VertexOutput output;
    output.position = mul(localToClipMatrix, pos);
    output.layer = shadowMapIndex;
    return output;
}