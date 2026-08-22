#pragma once
#include "vulkanRendererExport.h"
#include <cstdint>



namespace vulkanRendererBackend
{
	struct VULKAN_RENDERER_API GpuResourceHandle
	{
		uint32_t index;
		uint64_t generation;

		GpuResourceHandle();
		GpuResourceHandle(uint32_t index, uint64_t generation);

		bool IsValid() const;
		bool operator==(const GpuResourceHandle& other) const;
		bool operator!=(const GpuResourceHandle& other) const;
	};
}