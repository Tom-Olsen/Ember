#include "vulkanBufferCreateFlagsToString.h"
#include <sstream>



namespace emberVulkanUtility
{
	std::string ToString_VkBufferCreateFlags(VkBufferCreateFlags vkBufferCreateFlags)
	{
		if (vkBufferCreateFlags == 0)
			return "none";
		if (vkBufferCreateFlags == VK_BUFFER_CREATE_FLAG_BITS_MAX_ENUM)
			return "VK_BUFFER_CREATE_FLAG_BITS_MAX_ENUM";

		std::ostringstream ss;
		bool first = true;
		VkBufferCreateFlags knownFlags = 0;
		auto append = [&](VkBufferCreateFlagBits bit, const char* name)
		{
			knownFlags |= bit;
			if (vkBufferCreateFlags & bit)
			{
				if (!first)
					ss << " | ";
				first = false;
				ss << name;
			}
		};

		append(VK_BUFFER_CREATE_SPARSE_BINDING_BIT, "VK_BUFFER_CREATE_SPARSE_BINDING_BIT");
		append(VK_BUFFER_CREATE_SPARSE_RESIDENCY_BIT, "VK_BUFFER_CREATE_SPARSE_RESIDENCY_BIT");
		append(VK_BUFFER_CREATE_SPARSE_ALIASED_BIT, "VK_BUFFER_CREATE_SPARSE_ALIASED_BIT");
		append(VK_BUFFER_CREATE_PROTECTED_BIT, "VK_BUFFER_CREATE_PROTECTED_BIT");
		append(VK_BUFFER_CREATE_DEVICE_ADDRESS_CAPTURE_REPLAY_BIT, "VK_BUFFER_CREATE_DEVICE_ADDRESS_CAPTURE_REPLAY_BIT");
		append(VK_BUFFER_CREATE_DESCRIPTOR_BUFFER_CAPTURE_REPLAY_BIT_EXT, "VK_BUFFER_CREATE_DESCRIPTOR_BUFFER_CAPTURE_REPLAY_BIT_EXT");
		append(VK_BUFFER_CREATE_VIDEO_PROFILE_INDEPENDENT_BIT_KHR, "VK_BUFFER_CREATE_VIDEO_PROFILE_INDEPENDENT_BIT_KHR");

		VkBufferCreateFlags unknownFlags = vkBufferCreateFlags & ~knownFlags;
		if (unknownFlags != 0)
		{
			if (!first)
				ss << " | ";
			ss << "UNKNOWN_BUFFER_CREATE_BITS(0x" << std::hex << static_cast<uint32_t>(unknownFlags) << std::dec << ")";
		}

		return ss.str();
	}
}