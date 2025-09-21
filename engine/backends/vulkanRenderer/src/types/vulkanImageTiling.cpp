#include "vulkanImageTiling.h"
#include <unordered_map>
#include <vulkan\vulkan.h>



namespace vulkanRendererBackend
{
    ImageTiling ImageTilings::optimal = VK_IMAGE_TILING_OPTIMAL;
    ImageTiling ImageTilings::linear = VK_IMAGE_TILING_LINEAR;
    ImageTiling ImageTilings::drm_format_modifier_ext = VK_IMAGE_TILING_DRM_FORMAT_MODIFIER_EXT;
    ImageTiling ImageTilings::max_enum = VK_IMAGE_TILING_MAX_ENUM;

    std::string ImageTilings::ToString(ImageTiling imageTiling)
    {
        const static std::unordered_map<ImageTiling, std::string> imageTilingNames =
        {
            { ImageTilings::optimal, "optimal" },
            { ImageTilings::linear, "linear" },
            { ImageTilings::drm_format_modifier_ext, "drm_format_modifier_ext" },
            { ImageTilings::max_enum, "max_enum" }
        };

        auto it = imageTilingNames.find(imageTiling);
        return it != imageTilingNames.end() ? it->second : "unknown";
    }
}