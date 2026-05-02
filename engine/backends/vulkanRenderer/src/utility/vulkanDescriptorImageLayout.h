#pragma once
#include <vulkan/vulkan.h>



namespace vulkanRendererBackend
{
	[[nodiscard]] VkImageLayout GetDescriptorImageLayout(VkDescriptorType descriptorType);
}