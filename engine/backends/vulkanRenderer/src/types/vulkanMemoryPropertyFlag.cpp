#include "vulkanMemoryPropertyFlag.h"
#include <vulkan\vulkan.h>



namespace vulkanRendererBackend
{
    MemoryPropertyFlag MemoryPropertyFlags::device_local_bit = VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT;
    MemoryPropertyFlag MemoryPropertyFlags::host_visible_bit = VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT;
    MemoryPropertyFlag MemoryPropertyFlags::host_coherent_bit = VK_MEMORY_PROPERTY_HOST_COHERENT_BIT;
    MemoryPropertyFlag MemoryPropertyFlags::host_cached_bit = VK_MEMORY_PROPERTY_HOST_CACHED_BIT;
    MemoryPropertyFlag MemoryPropertyFlags::lazily_allocated_bit = VK_MEMORY_PROPERTY_LAZILY_ALLOCATED_BIT;
    MemoryPropertyFlag MemoryPropertyFlags::protected_bit = VK_MEMORY_PROPERTY_PROTECTED_BIT;
    MemoryPropertyFlag MemoryPropertyFlags::device_coherent_bit_amd = VK_MEMORY_PROPERTY_DEVICE_COHERENT_BIT_AMD;
    MemoryPropertyFlag MemoryPropertyFlags::device_uncached_bit_amd = VK_MEMORY_PROPERTY_DEVICE_UNCACHED_BIT_AMD;
    MemoryPropertyFlag MemoryPropertyFlags::rdma_capable_bit_nv = VK_MEMORY_PROPERTY_RDMA_CAPABLE_BIT_NV;
	MemoryPropertyFlag MemoryPropertyFlags::flag_bits_max_enum = VK_MEMORY_PROPERTY_FLAG_BITS_MAX_ENUM;

	std::string MemoryPropertyFlags::ToString(MemoryPropertyFlag memoryPropertyFlag)
	{
		if (memoryPropertyFlag == 0)
			return "none";

		std::string result;

		auto append = [&](MemoryPropertyFlag bit, std::string name)
		{
			if (memoryPropertyFlag & bit)
			{
				if (!result.empty()) result += " | ";
				result += name;
			}
		};

		append(MemoryPropertyFlags::device_local_bit, "device_local_bit");
		append(MemoryPropertyFlags::host_visible_bit, "host_visible_bit");
		append(MemoryPropertyFlags::host_coherent_bit, "host_coherent_bit");
		append(MemoryPropertyFlags::host_cached_bit, "host_cached_bit");
		append(MemoryPropertyFlags::lazily_allocated_bit, "lazily_allocated_bit");
		append(MemoryPropertyFlags::protected_bit, "protected_bit");
		append(MemoryPropertyFlags::device_coherent_bit_amd, "device_coherent_bit_amd");
		append(MemoryPropertyFlags::device_uncached_bit_amd, "device_uncached_bit_amd");
		append(MemoryPropertyFlags::rdma_capable_bit_nv, "rdma_capable_bit_nv");

		return result.empty() ? "unknown" : result;
	}
}