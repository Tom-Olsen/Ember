#include "vulkanPresentMode.h"
#include <unordered_map>
#include <vulkan/vulkan.h>



namespace vulkanRendererBackend
{
    PresentMode PresentModes::immediate = VK_PRESENT_MODE_IMMEDIATE_KHR;
    PresentMode PresentModes::mailbox = VK_PRESENT_MODE_MAILBOX_KHR;
    PresentMode PresentModes::fifo = VK_PRESENT_MODE_FIFO_KHR;
    PresentMode PresentModes::fifo_relaxed = VK_PRESENT_MODE_FIFO_RELAXED_KHR;
    PresentMode PresentModes::shared_demand_refresh = VK_PRESENT_MODE_SHARED_DEMAND_REFRESH_KHR;
    PresentMode PresentModes::shared_continuous_refresh = VK_PRESENT_MODE_SHARED_CONTINUOUS_REFRESH_KHR;
    PresentMode PresentModes::fifo_latest_ready = VK_PRESENT_MODE_FIFO_LATEST_READY_EXT;
    PresentMode PresentModes::max_enum = VK_PRESENT_MODE_MAX_ENUM_KHR;

    std::string PresentModes::ToString(PresentMode presentMode)
    {
        const static std::unordered_map<PresentMode, std::string> presentModeNames =
        {
            { PresentModes::immediate, "immediate" },
            { PresentModes::mailbox, "mailbox" },
            { PresentModes::fifo, "fifo" },
            { PresentModes::fifo_relaxed, "fifo_relaxed" },
            { PresentModes::shared_demand_refresh, "shared_demand_refresh" },
            { PresentModes::shared_continuous_refresh, "shared_continuous_refresh" },
            { PresentModes::fifo_latest_ready, "fifo_latest_ready" },
            { PresentModes::max_enum, "max_enum" }
        };

        auto it = presentModeNames.find(presentMode);
        return it != presentModeNames.end() ? it->second : "unknown";
    }
}