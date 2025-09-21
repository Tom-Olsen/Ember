#pragma once
#include <cstdint>
#include <string>



namespace vulkanRendererBackend
{
    using ImageViewType = uint32_t;
    struct ImageViewTypes
    {
        static ImageViewType view_type_1d;
        static ImageViewType view_type_2d;
        static ImageViewType view_type_3d;
        static ImageViewType view_type_cube;
        static ImageViewType view_type_1d_array;
        static ImageViewType view_type_2d_array;
        static ImageViewType view_type_cube_array;
        static ImageViewType view_type_max_enum;
        static std::string ToString(ImageViewType imageViewType);
    };
}