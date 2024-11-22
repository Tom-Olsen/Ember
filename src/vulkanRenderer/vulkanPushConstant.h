#pragma once
#ifndef __INCLUDE_GUARD_vulkanPushConstant_h__
#define __INCLUDE_GUARD_vulkanPushConstant_h__
#include "mathf.h"
#include <string>



/// <summary>
/// Size limit for push constants is 128 bytes.
/// Only used for small data that is updated every frame.
/// </summary>
struct VulkanPushConstant
{
private:
	alignas(4) float time;
	alignas(4) float deltaTime;
	alignas(4) int dLightsCount;
	alignas(4) int sLightsCount;
	alignas(4) int pLightsCount;

private:
	char padding[128 - 5 * sizeof(float)];

public:
	VulkanPushConstant(float time, float deltaTime, int dLightsCount, int sLightsCount, int pLightsCount);
	std::string ToString();
};



#endif // __INCLUDE_GUARD_vulkanPushConstant_h__