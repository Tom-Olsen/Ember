#include "computePushConstant.hlsli"
#include "mathf.hlsli"



RWTexture2D<float4> inputImage : register(u0);
RWTexture2D<float4> outputImage : register(u1);



[numthreads(32, 32, 1)]
void main(uint3 threadID : SV_DispatchThreadID)
{
    if (threadID.x < pc.threadCount.x && threadID.y < pc.threadCount.y)
    {
        float4 color = inputImage[threadID.xy];
        //if ((threadID.y / 10)  % 2 == 0)
        //    color.x = 0;
        //else
        //    color.y = 0;
        
        outputImage[threadID.xy] = color;
    }
}