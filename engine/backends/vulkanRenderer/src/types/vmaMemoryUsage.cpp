#include "vmaMemoryUsage.h"
#include "vk_mem_alloc.h"
#include <unordered_map>



namespace vulkanRendererBackend
{
    MemoryUsage MemoryUsages::unknown = VMA_MEMORY_USAGE_UNKNOWN;
    MemoryUsage MemoryUsages::gpu_only = VMA_MEMORY_USAGE_GPU_ONLY;
    MemoryUsage MemoryUsages::cpu_only = VMA_MEMORY_USAGE_CPU_ONLY;
    MemoryUsage MemoryUsages::cpu_to_gpu = VMA_MEMORY_USAGE_CPU_TO_GPU;
    MemoryUsage MemoryUsages::gpu_to_cpu = VMA_MEMORY_USAGE_GPU_TO_CPU;
    MemoryUsage MemoryUsages::cpu_copy = VMA_MEMORY_USAGE_CPU_COPY;
    MemoryUsage MemoryUsages::gpu_lazily_allocated = VMA_MEMORY_USAGE_GPU_LAZILY_ALLOCATED;
    MemoryUsage MemoryUsages::auto_any = VMA_MEMORY_USAGE_AUTO;
    MemoryUsage MemoryUsages::auto_prefer_device = VMA_MEMORY_USAGE_AUTO_PREFER_DEVICE;
    MemoryUsage MemoryUsages::auto_prefer_host = VMA_MEMORY_USAGE_AUTO_PREFER_HOST;
	MemoryUsage MemoryUsages::max_enum = VMA_MEMORY_USAGE_MAX_ENUM;

    std::string MemoryUsages::ToString(MemoryUsage nemoryUsage)
    {
        const static std::unordered_map<MemoryUsage, std::string> nemoryUsageNames =
        {
            { MemoryUsages::unknown, "unknown" },
            { MemoryUsages::gpu_only, "gpu_only" },
            { MemoryUsages::cpu_only, "cpu_only" },
            { MemoryUsages::cpu_to_gpu, "cpu_to_gpu" },
            { MemoryUsages::gpu_to_cpu, "gpu_to_cpu" },
            { MemoryUsages::cpu_copy, "cpu_copy" },
            { MemoryUsages::gpu_lazily_allocated, "gpu_lazily_allocated" },
            { MemoryUsages::auto_any, "auto_any" },
            { MemoryUsages::auto_prefer_device, "auto_prefer_device" },
            { MemoryUsages::auto_prefer_host, "auto_prefer_host" },
            { MemoryUsages::max_enum, "max_enum" }
        };

        auto it = nemoryUsageNames.find(nemoryUsage);
        return it != nemoryUsageNames.end() ? it->second : "unknown";
    }
}