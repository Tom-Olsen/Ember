#include "vulkanSharingMode.h"
#include <unordered_map>
#include <vulkan\vulkan.h>



namespace vulkanRendererBackend
{
    SharingMode SharingModes::exclusive = VK_SHARING_MODE_EXCLUSIVE;
    SharingMode SharingModes::concurrent = VK_SHARING_MODE_CONCURRENT;
    SharingMode SharingModes::max_enum = VK_SHARING_MODE_MAX_ENUM;

    std::string SharingModes::ToString(SharingMode sharingMode)
    {
        const static std::unordered_map<SharingMode, std::string> sharingModeNames =
        {
            { SharingModes::exclusive, "exclusive" },
            { SharingModes::concurrent, "concurrent" },
            { SharingModes::max_enum, "max_enum" }
        };

        auto it = sharingModeNames.find(sharingMode);
        return it != sharingModeNames.end() ? it->second : "unknown";
    }
}