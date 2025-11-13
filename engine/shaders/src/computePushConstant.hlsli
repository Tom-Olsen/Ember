#pragma once



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