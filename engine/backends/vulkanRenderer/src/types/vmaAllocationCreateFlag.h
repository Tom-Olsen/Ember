#pragma once
#include <cstdint>
#include <string>



namespace vulkanRendererBackend
{
    using AllocationCreateFlag = uint32_t;
    struct AllocationCreateFlags
    {
        static AllocationCreateFlag dedicated_memory_bit;
        static AllocationCreateFlag never_allocate_bit;
        static AllocationCreateFlag mapped_bit;
        static AllocationCreateFlag user_data_copy_string_bit;
        static AllocationCreateFlag upper_address_bit;
        static AllocationCreateFlag dont_bind_bit;
        static AllocationCreateFlag within_budget_bit;
        static AllocationCreateFlag can_alias_bit;
        static AllocationCreateFlag host_access_sequential_write_bit;
        static AllocationCreateFlag host_access_random_bit;
        static AllocationCreateFlag host_access_allow_transfer_instead_bit;
        static AllocationCreateFlag strategy_min_memory_bit;
        static AllocationCreateFlag strategy_min_time_bit;
        static AllocationCreateFlag strategy_min_offset_bit;
        static AllocationCreateFlag strategy_best_fit_bit;
        static AllocationCreateFlag strategy_first_fit_bit;
        static AllocationCreateFlag strategy_mask;
        static AllocationCreateFlag flag_bits_max_enum;
        static std::string ToString(AllocationCreateFlag allocationCreateFlag);
    };
}