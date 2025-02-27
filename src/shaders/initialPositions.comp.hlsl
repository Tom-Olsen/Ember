#include "computePushConstant.hlsli"
#include "mathf.hlsli"



struct InstanceData
{
    float4x4 localToWorldMatrix;
};
RWStructuredBuffer<InstanceData> instanceBuffer : register(u0);



[numthreads(64, 1, 1)]
void main(uint3 threadID : SV_DispatchThreadID)
{
    uint index = threadID.x;
    if (index < pc.threadCount.x)
    {
        float3 start = float3(-10, 0, 2);
        float3 end = float3(10, 0, 2);
        float3 position = (end - start) * ((float) index / (float) pc.threadCount.x) + start;
        float4x4 translation = LinAlg_Translate(position);
        float4x4 rotation = mathf_identity4x4;
        float4x4 scale = LinAlg_Scale(1.0f);
        instanceBuffer[index].localToWorldMatrix = mul(translation, mul(rotation, scale));
    }
}