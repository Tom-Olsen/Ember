#pragma once
#include <cstdint>
#include <string>



namespace vulkanRendererBackend
{
    using MemoryUsage = uint32_t;
    struct MemoryUsages
    {
        static MemoryUsage unknown;
        static MemoryUsage gpu_only;
        static MemoryUsage cpu_only;
        static MemoryUsage cpu_to_gpu;
        static MemoryUsage gpu_to_cpu;
        static MemoryUsage cpu_copy;
        static MemoryUsage gpu_lazily_allocated;
        static MemoryUsage auto_any;
        static MemoryUsage auto_prefer_device;
        static MemoryUsage auto_prefer_host;
        static MemoryUsage max_enum;
        static std::string ToString(MemoryUsage memoryUsage);
    };
}