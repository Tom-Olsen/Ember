#define EMBER_SCENE_COLOR_ACCESS EMBER_SCENE_COLOR_ACCESS_OUT_OF_PLACE
#include "computeShaderCommon.hlsli"



[numthreads(32, 32, 1)]
void main(uint3 threadID : SV_DispatchThreadID)
{
    if (threadID.x < pc.threadCount.x && threadID.y < pc.threadCount.y)
    {
        float4 color = GetSceneColor(threadID.xy);
        //if ((threadID.y / 10)  % 2 == 0)
        //    color.x = 0;
        //else
        //    color.y = 0;
        
        SetSceneColor(threadID.xy, color);
    }
}