#pragma once
#include "vmaAllocationCreateFlag.h"
#include "vmaMemoryUsage.h"
#include <cstdint>
#include <string>
#include <vulkan/vulkan.h>



// Forward declaration:
typedef struct VmaPool_T* VmaPool;



namespace vulkanRendererBackend
{
	struct AllocationCreateInfo
	{
		AllocationCreateFlag flags;
		MemoryUsage usages;
		VkMemoryPropertyFlags requiredFlags;
		VkMemoryPropertyFlags preferredFlags;
		uint32_t memoryTypeBits;
		VmaPool pool;
		void* pUserData;
		float priority;
		std::string ToString() const;
	};
}