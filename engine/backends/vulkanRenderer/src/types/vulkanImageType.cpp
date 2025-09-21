#include "vulkanImageType.h"
#include <unordered_map>
#include <vulkan\vulkan.h>



namespace vulkanRendererBackend
{
    
    ImageType ImageTypes::image_type_1d = VK_IMAGE_TYPE_1D;
    ImageType ImageTypes::image_type_2d = VK_IMAGE_TYPE_2D;
    ImageType ImageTypes::image_type_3d = VK_IMAGE_TYPE_3D;
    ImageType ImageTypes::image_type_max_enum = VK_IMAGE_TYPE_MAX_ENUM;

    std::string ImageTypes::ToString(ImageType imageType)
    {
        const static std::unordered_map<ImageType, std::string> imageTypeNames =
        {
            { ImageTypes::image_type_1d, "image_type_1d" },
            { ImageTypes::image_type_2d, "image_type_2d" },
            { ImageTypes::image_type_3d, "image_type_3d" },
            { ImageTypes::image_type_max_enum, "image_type_max_enum" }
        };

        auto it = imageTypeNames.find(imageType);
        return it != imageTypeNames.end() ? it->second : "unknown";
    }
}