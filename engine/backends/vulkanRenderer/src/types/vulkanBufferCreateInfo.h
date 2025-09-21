#pragma once
#include "vulkanBufferCreateFlag.h"
#include "vulkanBufferUsageFlag.h"
#include "vulkanSharingMode.h"
#include <cstdint>
#include <string>



namespace vulkanRendererBackend
{
	struct BufferCreateInfo
	{
		void* pNext;
		BufferCreateFlag flags;
		uint64_t size;
		BufferUsageFlag usages;
		SharingMode sharingMode;
		uint32_t queueFamilyIndexCount;
		const uint32_t* pQueueFamilyIndices;
		std::string ToString() const;
	};
}