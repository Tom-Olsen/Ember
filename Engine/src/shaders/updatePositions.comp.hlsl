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
        float t01 = (float) index / (float) pc.threadCount.x;
        float3 start = float3(-10, 0, 2);
        float3 end = float3(10, 0, 2);
        float3 position = (end - start) * t01 + start;
        position.z += sin(4.0f * math_PI * t01 + pc.time);
        float4x4 translation = LinAlg_Translate(position);
        float4x4 rotation = math_identity4x4;
        float4x4 scale = LinAlg_Scale(1.0f);
        float4x4 TRS = mul(translation, mul(rotation, scale));
        instanceBuffer[index].localToWorldMatrix = TRS;
        float4 color = instanceBuffer[index].color;
        color.b = 0.5 * sin(math_PI * pc.time) + 0.5;
        instanceBuffer[index].color = color;
    }
}