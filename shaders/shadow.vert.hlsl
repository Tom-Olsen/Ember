#include "shadowMapping.hlsli"



struct PushConstant
{
    float time;
    float delaTime;
    int dLightsCount;
    int sLightsCount;
    int pLightsCount;
};
#if defined(_DXC)
[[vk::push_constant]] CullPushConstants pc;
#else
[[vk::push_constant]] ConstantBuffer<PushConstant> pc;
#endif



cbuffer LightMatrizes : register(b0)
{
    // local to camera clip space matrix: (projection * view * model)
    float4x4 localToClipMatrix[MAX_D_LIGHTS + MAX_S_LIGHTS];
};



struct VertexInput
{
    float3 position : POSITION;
    uint layerIndex : SV_InstanceID; // Instance index used to select the depth layer
};

struct VertexOutput
{
    float4 position : SV_POSITION;
    uint layer : SV_RenderTargetArrayIndex;
};



VertexOutput main(VertexInput input)
{
    float4 pos = float4(input.position, 1.0);
    pos.w = 1.0f;
    
    uint index = 0;
    if (index < pc.dLightsCount)
        index = input.layerIndex;
    else
        index = input.layerIndex + MAX_D_LIGHTS - pc.dLightsCount;
    
    VertexOutput output;
    output.position = mul(localToClipMatrix[index], pos);
    output.layer = input.layerIndex;
    return output;
}