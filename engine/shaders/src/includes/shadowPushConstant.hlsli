#ifndef __INCLUDE_GUARD_shadowPushConstant_hlsli__
#define __INCLUDE_GUARD_shadowPushConstant_hlsli__



struct ShadowPushConstant
{
    // ._41 = instanceCount, _42 = shadowMapIndex, rest = localToWorldMatrix (with _41_42 rest to 00)
    float4x4 instanceCountAndShadowMapIndexAndLocalToWorldMatrix;
    float4x4 worldToClipMatrix;
};
#if defined(_DXC)
[[vk::push_constant]] ShadowPushConstant pc;
#else
[[vk::push_constant]] ConstantBuffer<ShadowPushConstant> pc;
#endif



#endif // __INCLUDE_GUARD_shadowPushConstant_hlsli__