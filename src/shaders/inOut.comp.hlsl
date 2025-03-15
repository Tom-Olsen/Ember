#include "computePushConstant.hlsli"
#include "mathf.hlsli"



RWTexture2D<float4> inputImage : register(u0);
RWTexture2D<float4> outputImage : register(u1);



[numthreads(32, 32, 1)]
void main(uint3 threadID : SV_DispatchThreadID)
{
    if (threadID.x < pc.threadCount.x && threadID.y < pc.threadCount.y)
        outputImage[threadID.xy] = inputImage[threadID.xy];
}