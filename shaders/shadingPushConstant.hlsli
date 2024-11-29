#ifndef __INCLUDE_GUARD_shadingPushConstant_hlsli__
#define __INCLUDE_GUARD_shadingPushConstant_hlsli__



struct ShadingPushConstant
{
    float time;
    float delaTime;
    int dLightsCount;
    int sLightsCount;
    int pLightsCount;
    float4 cameraPosition;
};
#if defined(_DXC)
[[vk::push_constant]] ShadingPushConstant pc;
#else
[[vk::push_constant]] ConstantBuffer<ShadingPushConstant> pc;
#endif



#endif //__INCLUDE_GUARD_shadingPushConstant_hlsli__