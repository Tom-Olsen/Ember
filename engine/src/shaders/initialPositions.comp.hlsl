#include "computePushConstant.hlsli"
#include "math.hlsli"



struct InstanceData
{
    float4x4 localToWorldMatrix;
    float4 color;
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
        float t01 = (float) index / (float) pc.threadCount.x;
        float3 position = (end - start) * t01 + start;
        float4x4 translation = LinAlg_Translate(position);
        float4x4 rotation = math_identity4x4;
        float4x4 scale = LinAlg_Scale(1.0f);
        instanceBuffer[index].localToWorldMatrix = mul(translation, mul(rotation, scale));
        instanceBuffer[index].color = float4(t01, 1, 1, 1);
    }
}