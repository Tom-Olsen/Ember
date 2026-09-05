#pragma once
#include <spirv_reflect.h>
#include <vulkan/vulkan.h>



namespace emberSpirvReflect
{
	VkFormat ImageFormatSpirvToVulkan(SpvImageFormat imageFormat);
}