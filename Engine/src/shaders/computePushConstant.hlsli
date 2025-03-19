#ifndef __INCLUDE_GUARD_computePushConstant_hlsli__
#define __INCLUDE_GUARD_computePushConstant_hlsli__



struct ComputePushConstant
{
    uint3 threadCount;
    float time;
    float deltaTime;
};
#if defined(_DXC)
[[vk::push_constant]] ComputePushConstant pc;
#else
[[vk::push_constant]] ConstantBuffer<ComputePushConstant> pc;
#endif



#endif //__INCLUDE_GUARD_computePushConstant_hlsli__