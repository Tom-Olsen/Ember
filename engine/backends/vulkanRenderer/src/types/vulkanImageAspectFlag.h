#pragma once
#include <cstdint>
#include <string>



namespace vulkanRendererBackend
{
    using ImageAspectFlag = uint32_t;
    struct ImageAspectFlags
    {
        static ImageAspectFlag none;
        static ImageAspectFlag color_bit;
        static ImageAspectFlag depth_bit;
        static ImageAspectFlag stencil_bit;
        static ImageAspectFlag metadata_bit;
        static ImageAspectFlag plane_0_bit;
        static ImageAspectFlag plane_1_bit;
        static ImageAspectFlag plane_2_bit;
        static ImageAspectFlag memory_plane_0_bit_ext;
        static ImageAspectFlag memory_plane_1_bit_ext;
        static ImageAspectFlag memory_plane_2_bit_ext;
        static ImageAspectFlag memory_plane_3_bit_ext;
        static ImageAspectFlag flag_bits_max_enum;
        static std::string ToString(ImageAspectFlag imageAspectFlag);
    };
}