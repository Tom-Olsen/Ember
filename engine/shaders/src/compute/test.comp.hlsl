#include "descriptorSetMacros.h"
#include "computePushConstant.hlsli"
#include "math.hlsli"



StructuredBuffer<float> dataBuffer : register(t0, SHADER_SET);
Texture2D<float4> inputImage : register(t1, SHADER_SET);
RWTexture2D<float4> outputImage : register(u2, SHADER_SET);



cbuffer Properties : register(b3, SHADER_SET)
{
    uint a;
    uint b;
};



[numthreads(32, 32, 1)]
void main(uint3 threadID : SV_DispatchThreadID)
{
    if (threadID.x < pc.threadCount.x && threadID.y < pc.threadCount.y)
    {
        float4 data = dataBuffer[threadID.x].xxxx;
        float4 color = inputImage[threadID.xy];
        outputImage[threadID.xy] = a * data + b * color;
    }
}