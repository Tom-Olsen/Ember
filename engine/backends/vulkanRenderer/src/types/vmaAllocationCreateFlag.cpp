#include "vmaAllocationCreateFlag.h"
#include "vk_mem_alloc.h"



namespace vulkanRendererBackend
{
	AllocationCreateFlag AllocationCreateFlags::dedicated_memory_bit = VMA_ALLOCATION_CREATE_DEDICATED_MEMORY_BIT;
	AllocationCreateFlag AllocationCreateFlags::never_allocate_bit = VMA_ALLOCATION_CREATE_NEVER_ALLOCATE_BIT;
	AllocationCreateFlag AllocationCreateFlags::mapped_bit = VMA_ALLOCATION_CREATE_MAPPED_BIT;
	AllocationCreateFlag AllocationCreateFlags::user_data_copy_string_bit = VMA_ALLOCATION_CREATE_USER_DATA_COPY_STRING_BIT;
	AllocationCreateFlag AllocationCreateFlags::upper_address_bit = VMA_ALLOCATION_CREATE_UPPER_ADDRESS_BIT;
	AllocationCreateFlag AllocationCreateFlags::dont_bind_bit = VMA_ALLOCATION_CREATE_DONT_BIND_BIT;
	AllocationCreateFlag AllocationCreateFlags::within_budget_bit = VMA_ALLOCATION_CREATE_WITHIN_BUDGET_BIT;
	AllocationCreateFlag AllocationCreateFlags::can_alias_bit = VMA_ALLOCATION_CREATE_CAN_ALIAS_BIT;
	AllocationCreateFlag AllocationCreateFlags::host_access_sequential_write_bit = VMA_ALLOCATION_CREATE_HOST_ACCESS_SEQUENTIAL_WRITE_BIT;
	AllocationCreateFlag AllocationCreateFlags::host_access_random_bit = VMA_ALLOCATION_CREATE_HOST_ACCESS_RANDOM_BIT;
	AllocationCreateFlag AllocationCreateFlags::host_access_allow_transfer_instead_bit = VMA_ALLOCATION_CREATE_HOST_ACCESS_ALLOW_TRANSFER_INSTEAD_BIT;
	AllocationCreateFlag AllocationCreateFlags::strategy_min_memory_bit = VMA_ALLOCATION_CREATE_STRATEGY_MIN_MEMORY_BIT;
	AllocationCreateFlag AllocationCreateFlags::strategy_min_time_bit = VMA_ALLOCATION_CREATE_STRATEGY_MIN_TIME_BIT;
	AllocationCreateFlag AllocationCreateFlags::strategy_min_offset_bit = VMA_ALLOCATION_CREATE_STRATEGY_MIN_OFFSET_BIT;
	AllocationCreateFlag AllocationCreateFlags::strategy_best_fit_bit = VMA_ALLOCATION_CREATE_STRATEGY_BEST_FIT_BIT;
	AllocationCreateFlag AllocationCreateFlags::strategy_first_fit_bit = VMA_ALLOCATION_CREATE_STRATEGY_FIRST_FIT_BIT;
	AllocationCreateFlag AllocationCreateFlags::strategy_mask = VMA_ALLOCATION_CREATE_STRATEGY_MASK;
	AllocationCreateFlag AllocationCreateFlags::flag_bits_max_enum = VMA_ALLOCATION_CREATE_FLAG_BITS_MAX_ENUM;

	std::string AllocationCreateFlags::ToString(AllocationCreateFlag allocationCreateFlag)
	{
		if (allocationCreateFlag == 0)
			return "none";

		std::string result;

		auto append = [&](AllocationCreateFlag bit, std::string name)
		{
			if (allocationCreateFlag & bit)
			{
				if (!result.empty()) result += " | ";
				result += name;
			}
		};

		append(AllocationCreateFlags::dedicated_memory_bit, "dedicated_memory_bit");
		append(AllocationCreateFlags::never_allocate_bit, "never_allocate_bit");
		append(AllocationCreateFlags::mapped_bit, "mapped_bit");
		append(AllocationCreateFlags::user_data_copy_string_bit, "user_data_copy_string_bit");
		append(AllocationCreateFlags::upper_address_bit, "upper_address_bit");
		append(AllocationCreateFlags::dont_bind_bit, "dont_bind_bit");
		append(AllocationCreateFlags::within_budget_bit, "within_budget_bit");
		append(AllocationCreateFlags::can_alias_bit, "can_alias_bit");
		append(AllocationCreateFlags::host_access_sequential_write_bit, "host_access_sequential_write_bit");
		append(AllocationCreateFlags::host_access_random_bit, "host_access_random_bit");
		append(AllocationCreateFlags::host_access_allow_transfer_instead_bit, "host_access_allow_transfer_instead_bit");
		append(AllocationCreateFlags::strategy_min_memory_bit, "strategy_min_memory_bit");
		append(AllocationCreateFlags::strategy_min_time_bit, "strategy_min_time_bit");
		append(AllocationCreateFlags::strategy_min_offset_bit, "strategy_min_offset_bit");
		append(AllocationCreateFlags::strategy_best_fit_bit, "strategy_best_fit_bit");
		append(AllocationCreateFlags::strategy_first_fit_bit, "strategy_first_fit_bit");

		return result.empty() ? "unknown" : result;
	}
}