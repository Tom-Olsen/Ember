#pragma once
#include <cstdint>
#include <string>



namespace vulkanRendererBackend
{
    using MemoryPropertyFlag = uint32_t;
    struct MemoryPropertyFlags
    {
        static MemoryPropertyFlag device_local_bit;
        static MemoryPropertyFlag host_visible_bit;
        static MemoryPropertyFlag host_coherent_bit;
        static MemoryPropertyFlag host_cached_bit;
        static MemoryPropertyFlag lazily_allocated_bit;
        static MemoryPropertyFlag protected_bit;
        static MemoryPropertyFlag device_coherent_bit_amd;
        static MemoryPropertyFlag device_uncached_bit_amd;
        static MemoryPropertyFlag rdma_capable_bit_nv;
        static MemoryPropertyFlag flag_bits_max_enum;
        static std::string ToString(MemoryPropertyFlag memoryPropertyFlag);
    };
}