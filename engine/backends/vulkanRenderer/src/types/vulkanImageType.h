#pragma once
#include <cstdint>
#include <string>



namespace vulkanRendererBackend
{
	using ImageType = uint32_t;
	struct ImageTypes
	{
		static ImageType image_type_1d;
		static ImageType image_type_2d;
		static ImageType image_type_3d;
		static ImageType image_type_max_enum;
		static std::string ToString(ImageType imageType);
	};
}