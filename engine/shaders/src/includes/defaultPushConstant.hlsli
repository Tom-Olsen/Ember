#ifndef __INCLUDE_GUARD_defaultPushConstant_hlsli__
#define __INCLUDE_GUARD_defaultPushConstant_hlsli__



struct DefaultPushConstant
{
    uint targetIndex;       // renderTarget, shadowMap, cubeFace, etc. depending on context.
    uint instanceCount;     // 0 implies no instanced rendering.
    uint receiveShadows;    // 0 = no, other = yes
    float time;
    float deltaTime;
};
#if defined(_DXC)
[[vk::push_constant]] DefaultPushConstant pc;
#else
[[vk::push_constant]] ConstantBuffer<DefaultPushConstant> pc;
#endif



#endif // __INCLUDE_GUARD_defaultPushConstant_hlsli__