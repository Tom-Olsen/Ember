#ifndef __INCLUDE_GUARD_defaultPushConstant_hlsli__
#define __INCLUDE_GUARD_defaultPushConstant_hlsli__



struct DefaultPushConstant
{
    uint instanceCount; // 0 implies no instanced rendering.
    float time;
    float deltaTime;
    int dLightsCount;
    int pLightsCount;
    int sLightsCount;
    float4 cameraPosition;
};
#if defined(_DXC)
[[vk::push_constant]] DefaultPushConstant pc;
#else
[[vk::push_constant]] ConstantBuffer<DefaultPushConstant> pc;
#endif



#endif //__INCLUDE_GUARD_defaultPushConstant_hlsli__