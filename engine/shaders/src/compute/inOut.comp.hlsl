#include "computePushConstant.hlsli"
#include "descriptorSetMacros.h"
#include "math.hlsli"



RWTexture2D<float4> inputImage : register(u0, SHADER_SET);
RWTexture2D<float4> outputImage : register(u1, SHADER_SET);



[numthreads(32, 32, 1)]
void main(uint3 threadID : SV_DispatchThreadID)
{
    if (threadID.x < pc.threadCount.x && threadID.y < pc.threadCount.y)
        outputImage[threadID.xy] = inputImage[threadID.xy];
}