#include "vmaAllocationCreateFlagsToString.h"
#include <sstream>



namespace vulkanRendererBackend
{
	std::string ToString_VmaAllocationCreateFlags(VmaAllocationCreateFlags vmaAllocationCreateFlags)
	{
		if (vmaAllocationCreateFlags == 0)
			return "none";

		if (vmaAllocationCreateFlags == VMA_ALLOCATION_CREATE_FLAG_BITS_MAX_ENUM)
			return "VMA_ALLOCATION_CREATE_FLAG_BITS_MAX_ENUM";

		std::ostringstream ss;
		bool first = true;
		VmaAllocationCreateFlags knownFlags = 0;
		auto append = [&](VmaAllocationCreateFlagBits bit, const char* name)
		{
			knownFlags |= bit;
			if (vmaAllocationCreateFlags & bit)
			{
				if (!first)
					ss << " | ";
				first = false;
				ss << name;
			}
		};

		append(VMA_ALLOCATION_CREATE_DEDICATED_MEMORY_BIT, "VMA_ALLOCATION_CREATE_DEDICATED_MEMORY_BIT");
		append(VMA_ALLOCATION_CREATE_NEVER_ALLOCATE_BIT, "VMA_ALLOCATION_CREATE_NEVER_ALLOCATE_BIT");
		append(VMA_ALLOCATION_CREATE_MAPPED_BIT, "VMA_ALLOCATION_CREATE_MAPPED_BIT");
		append(VMA_ALLOCATION_CREATE_USER_DATA_COPY_STRING_BIT, "VMA_ALLOCATION_CREATE_USER_DATA_COPY_STRING_BIT");
		append(VMA_ALLOCATION_CREATE_UPPER_ADDRESS_BIT, "VMA_ALLOCATION_CREATE_UPPER_ADDRESS_BIT");
		append(VMA_ALLOCATION_CREATE_DONT_BIND_BIT, "VMA_ALLOCATION_CREATE_DONT_BIND_BIT");
		append(VMA_ALLOCATION_CREATE_WITHIN_BUDGET_BIT, "VMA_ALLOCATION_CREATE_WITHIN_BUDGET_BIT");
		append(VMA_ALLOCATION_CREATE_CAN_ALIAS_BIT, "VMA_ALLOCATION_CREATE_CAN_ALIAS_BIT");
		append(VMA_ALLOCATION_CREATE_HOST_ACCESS_SEQUENTIAL_WRITE_BIT, "VMA_ALLOCATION_CREATE_HOST_ACCESS_SEQUENTIAL_WRITE_BIT");
		append(VMA_ALLOCATION_CREATE_HOST_ACCESS_RANDOM_BIT, "VMA_ALLOCATION_CREATE_HOST_ACCESS_RANDOM_BIT");
		append(VMA_ALLOCATION_CREATE_HOST_ACCESS_ALLOW_TRANSFER_INSTEAD_BIT, "VMA_ALLOCATION_CREATE_HOST_ACCESS_ALLOW_TRANSFER_INSTEAD_BIT");
		append(VMA_ALLOCATION_CREATE_STRATEGY_MIN_MEMORY_BIT, "VMA_ALLOCATION_CREATE_STRATEGY_MIN_MEMORY_BIT");
		append(VMA_ALLOCATION_CREATE_STRATEGY_MIN_TIME_BIT, "VMA_ALLOCATION_CREATE_STRATEGY_MIN_TIME_BIT");
		append(VMA_ALLOCATION_CREATE_STRATEGY_MIN_OFFSET_BIT, "VMA_ALLOCATION_CREATE_STRATEGY_MIN_OFFSET_BIT");

		VmaAllocationCreateFlags unknownFlags = vmaAllocationCreateFlags & ~knownFlags;
		if (unknownFlags != 0)
		{
			if (!first)
				ss << " | ";
			ss << "UNKNOWN_VMA_ALLOCATION_CREATE_BITS(0x" << std::hex << unknownFlags << std::dec << ")";
		}

		return ss.str();
	}
}