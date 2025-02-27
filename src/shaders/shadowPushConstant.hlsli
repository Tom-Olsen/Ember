#ifndef __INCLUDE_GUARD_shadowPushConstant_hlsli__
#define __INCLUDE_GUARD_shadowPushConstant_hlsli__



struct ShadowPushConstant
{
    uint instanceCount; // 0 implies no instanced rendering.
    int shadowMapIndex;
    float4x4 localToClipMatrix;
};
#if defined(_DXC)
[[vk::push_constant]] ShadowPushConstant pc;
#else
[[vk::push_constant]] ConstantBuffer<ShadowPushConstant> pc;
#endif



#endif //__INCLUDE_GUARD_shadowPushConstant_hlsli__