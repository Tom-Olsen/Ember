#pragma once
#include "vmaAllocationCreateFlag.h"
#include "vmaMemoryUsage.h"
#include "vulkanMemoryPropertyFlag.h"
#include <cstdint>
#include <string>



// Forward decleration:
typedef struct VmaPool_T* VmaPool;



namespace vulkanRendererBackend
{
	struct AllocationCreateInfo
	{
		AllocationCreateFlag flags;
		MemoryUsage usages;
		MemoryPropertyFlag requiredFlags;
		MemoryPropertyFlag preferredFlags;
		uint32_t memoryTypeBits;
		VmaPool pool;
		void* pUserData;
		float priority;
		std::string ToString() const;
	};
}