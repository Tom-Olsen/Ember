#pragma once
#include "vulkanBufferCreateFlag.h"
#include "vulkanBufferUsageFlag.h"
#include <cstdint>
#include <string>
#include <vulkan/vulkan.h>



namespace vulkanRendererBackend
{
	struct BufferCreateInfo
	{
		void* pNext;
		BufferCreateFlag flags;
		uint64_t size;
		BufferUsageFlag usages;
		VkSharingMode sharingMode;
		uint32_t queueFamilyIndexCount;
		const uint32_t* pQueueFamilyIndices;
		std::string ToString() const;
	};
}