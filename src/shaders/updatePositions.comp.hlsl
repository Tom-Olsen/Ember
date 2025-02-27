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
        //float direction = sign(sin(pc.time));
        //float speed = 0.001f + 0.001f * ((float) index / (float) pc.threadCount.x);
        //float delta = direction * speed * pc.deltaTime;
        //float4x4 translation = LinAlg_Translate(float3(0, 0, delta));
        //instanceBuffer[index].localToWorldMatrix = mul(instanceBuffer[index].localToWorldMatrix, translation);
        
        instanceBuffer[index].localToWorldMatrix = instanceBuffer[index].localToWorldMatrix;
        
        //float3 start = float3(-10, 0, 2);
        //float3 end = float3(10, 0, 2);
        //float3 position = (end - start) * ((float) index / (float) pc.threadCount.x) + start + 0.1f * pc.time * float3(0, 0, 1);
        //float4x4 translation = LinAlg_Translate(position);
        //float4x4 rotation = mathf_identity4x4;
        //float4x4 scale = LinAlg_Scale(1.0f);
        //float4x4 TRS = mul(translation, mul(rotation, scale));
        //instanceBuffer[index].localToWorldMatrix = TRS;
    }
}