#ifndef __INCLUDE_GUARD_pushConstant_hlsli__
#define __INCLUDE_GUARD_pushConstant_hlsli__



struct PushConstant
{
    float time;
    float delaTime;
    int dLightsCount;
    int sLightsCount;
    int pLightsCount;
    float4 cameraPosition;
};
#if defined(_DXC)
[[vk::push_constant]] CullPushConstants pc;
#else
[[vk::push_constant]] ConstantBuffer<PushConstant> pc;
#endif



#endif //__INCLUDE_GUARD_pushConstant_hlsli__