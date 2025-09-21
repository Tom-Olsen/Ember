#include "vulkanImageViewType.h"
#include <unordered_map>
#include <vulkan/vulkan.h>



namespace vulkanRendererBackend
{
	ImageViewType ImageViewTypes::view_type_1d = VK_IMAGE_VIEW_TYPE_1D;
	ImageViewType ImageViewTypes::view_type_2d = VK_IMAGE_VIEW_TYPE_2D;
	ImageViewType ImageViewTypes::view_type_3d = VK_IMAGE_VIEW_TYPE_3D;
	ImageViewType ImageViewTypes::view_type_cube = VK_IMAGE_VIEW_TYPE_CUBE;
	ImageViewType ImageViewTypes::view_type_1d_array = VK_IMAGE_VIEW_TYPE_1D_ARRAY;
	ImageViewType ImageViewTypes::view_type_2d_array = VK_IMAGE_VIEW_TYPE_2D_ARRAY;
	ImageViewType ImageViewTypes::view_type_cube_array = VK_IMAGE_VIEW_TYPE_CUBE_ARRAY;
	ImageViewType ImageViewTypes::view_type_max_enum = VK_IMAGE_VIEW_TYPE_MAX_ENUM;

	std::string ImageViewTypes::ToString(ImageViewType imageViewType)
	{
		const static std::unordered_map<ImageViewType, std::string> imageViewTypeNames =
		{
			{ ImageViewTypes::view_type_1d, "viewType_1d" },
			{ ImageViewTypes::view_type_2d, "viewType_2d" },
			{ ImageViewTypes::view_type_3d, "viewType_3d" },
			{ ImageViewTypes::view_type_cube, "viewType_cube" },
			{ ImageViewTypes::view_type_1d_array, "viewType_1d_array" },
			{ ImageViewTypes::view_type_2d_array, "viewType_2d_array" },
			{ ImageViewTypes::view_type_cube_array, "viewType_cube_array" },
			{ ImageViewTypes::view_type_max_enum, "viewType_max_enum" }
		};

		auto it = imageViewTypeNames.find(imageViewType);
		return it != imageViewTypeNames.end() ? it->second : "unknown";
	}
}