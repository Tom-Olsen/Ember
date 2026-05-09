#pragma once
#include <cstdint>
#include <string>
#include <vulkan/vulkan.h>



namespace vulkanRendererBackend
{
	struct BufferCreateInfo
	{
		void* pNext;
		VkBufferCreateFlags flags;
		uint64_t size;
		VkBufferUsageFlags usages;
		VkSharingMode sharingMode;
		uint32_t queueFamilyIndexCount;
		const uint32_t* pQueueFamilyIndices;
		std::string ToString() const;
	};
}