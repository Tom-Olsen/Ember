#include "vmaMemoryUsageToString.h"



namespace vulkanRendererBackend
{
	std::string ToString_VmaMemoryUsage(VmaMemoryUsage vmaMemoryUsage)
	{
		switch (vmaMemoryUsage)
		{
			case VMA_MEMORY_USAGE_UNKNOWN:
				return "VMA_MEMORY_USAGE_UNKNOWN";
			case VMA_MEMORY_USAGE_GPU_ONLY:
				return "VMA_MEMORY_USAGE_GPU_ONLY";
			case VMA_MEMORY_USAGE_CPU_ONLY:
				return "VMA_MEMORY_USAGE_CPU_ONLY";
			case VMA_MEMORY_USAGE_CPU_TO_GPU:
				return "VMA_MEMORY_USAGE_CPU_TO_GPU";
			case VMA_MEMORY_USAGE_GPU_TO_CPU:
				return "VMA_MEMORY_USAGE_GPU_TO_CPU";
			case VMA_MEMORY_USAGE_CPU_COPY:
				return "VMA_MEMORY_USAGE_CPU_COPY";
			case VMA_MEMORY_USAGE_GPU_LAZILY_ALLOCATED:
				return "VMA_MEMORY_USAGE_GPU_LAZILY_ALLOCATED";
			case VMA_MEMORY_USAGE_AUTO:
				return "VMA_MEMORY_USAGE_AUTO";
			case VMA_MEMORY_USAGE_AUTO_PREFER_DEVICE:
				return "VMA_MEMORY_USAGE_AUTO_PREFER_DEVICE";
			case VMA_MEMORY_USAGE_AUTO_PREFER_HOST:
				return "VMA_MEMORY_USAGE_AUTO_PREFER_HOST";
			case VMA_MEMORY_USAGE_MAX_ENUM:
				return "VMA_MEMORY_USAGE_MAX_ENUM";
			default:
				return "unknown";
		}
	}
}