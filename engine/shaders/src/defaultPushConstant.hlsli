#pragma once



struct DefaultPushConstant
{
    uint instanceCount; // 0 implies no instanced rendering.
    float time;
    float deltaTime;
    int dLightsCount;
    int sLightsCount;
    float4 cameraPosition;
};
#if defined(_DXC)
[[vk::push_constant]] DefaultPushConstant pc;
#else
[[vk::push_constant]] ConstantBuffer<DefaultPushConstant> pc;
#endif