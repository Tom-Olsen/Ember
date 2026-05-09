#include "vulkanMemoryPropertyFlagsToString.h"
#include <sstream>



namespace emberVulkanUtility
{
	std::string ToString_VkMemoryPropertyFlags(VkMemoryPropertyFlags vkMemoryPropertyFlags)
	{
		if (vkMemoryPropertyFlags == 0)
			return "none";
		if (vkMemoryPropertyFlags == VK_MEMORY_PROPERTY_FLAG_BITS_MAX_ENUM)
			return "VK_MEMORY_PROPERTY_FLAG_BITS_MAX_ENUM";

		std::ostringstream ss;
		bool first = true;
		VkMemoryPropertyFlags knownFlags = 0;
		auto append = [&](VkMemoryPropertyFlagBits bit, const char* name)
		{
			knownFlags |= bit;
			if (vkMemoryPropertyFlags & bit)
			{
				if (!first)
					ss << " | ";
				first = false;
				ss << name;
			}
		};

		append(VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, "VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT");
		append(VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT, "VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT");
		append(VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, "VK_MEMORY_PROPERTY_HOST_COHERENT_BIT");
		append(VK_MEMORY_PROPERTY_HOST_CACHED_BIT, "VK_MEMORY_PROPERTY_HOST_CACHED_BIT");
		append(VK_MEMORY_PROPERTY_LAZILY_ALLOCATED_BIT, "VK_MEMORY_PROPERTY_LAZILY_ALLOCATED_BIT");
		append(VK_MEMORY_PROPERTY_PROTECTED_BIT, "VK_MEMORY_PROPERTY_PROTECTED_BIT");
		append(VK_MEMORY_PROPERTY_DEVICE_COHERENT_BIT_AMD, "VK_MEMORY_PROPERTY_DEVICE_COHERENT_BIT_AMD");
		append(VK_MEMORY_PROPERTY_DEVICE_UNCACHED_BIT_AMD, "VK_MEMORY_PROPERTY_DEVICE_UNCACHED_BIT_AMD");
		append(VK_MEMORY_PROPERTY_RDMA_CAPABLE_BIT_NV, "VK_MEMORY_PROPERTY_RDMA_CAPABLE_BIT_NV");

		VkMemoryPropertyFlags unknownFlags = vkMemoryPropertyFlags & ~knownFlags;
		if (unknownFlags != 0)
		{
			if (!first)
				ss << " | ";
			ss << "UNKNOWN_MEMORY_PROPERTY_BITS(0x" << std::hex << static_cast<uint32_t>(unknownFlags) << std::dec << ")";
		}

		return ss.str();
	}
}