#include "vulkanColorSpace.h"
#include <unordered_map>
#include <vulkan\vulkan.h>



namespace vulkanRendererBackend
{
    ColorSpace ColorSpaces::srgb_nonlinear = VK_COLOR_SPACE_SRGB_NONLINEAR_KHR;
    ColorSpace ColorSpaces::display_p3_nonlinear = VK_COLOR_SPACE_DISPLAY_P3_NONLINEAR_EXT;
    ColorSpace ColorSpaces::extended_srgb_linear = VK_COLOR_SPACE_EXTENDED_SRGB_LINEAR_EXT;
    ColorSpace ColorSpaces::display_p3_linear = VK_COLOR_SPACE_DISPLAY_P3_LINEAR_EXT;
    ColorSpace ColorSpaces::dci_p3_nonlinear = VK_COLOR_SPACE_DCI_P3_NONLINEAR_EXT;
    ColorSpace ColorSpaces::bt709_linear = VK_COLOR_SPACE_BT709_LINEAR_EXT;
    ColorSpace ColorSpaces::bt709_nonlinear = VK_COLOR_SPACE_BT709_NONLINEAR_EXT;
    ColorSpace ColorSpaces::bt2020_linear = VK_COLOR_SPACE_BT2020_LINEAR_EXT;
    ColorSpace ColorSpaces::hdr10_st2084 = VK_COLOR_SPACE_HDR10_ST2084_EXT;
    ColorSpace ColorSpaces::dolbyvision = VK_COLOR_SPACE_DOLBYVISION_EXT;
    ColorSpace ColorSpaces::hdr10_hlg = VK_COLOR_SPACE_HDR10_HLG_EXT;
    ColorSpace ColorSpaces::adobergb_linear = VK_COLOR_SPACE_ADOBERGB_LINEAR_EXT;
    ColorSpace ColorSpaces::adobergb_nonlinear = VK_COLOR_SPACE_ADOBERGB_NONLINEAR_EXT;
    ColorSpace ColorSpaces::pass_through = VK_COLOR_SPACE_PASS_THROUGH_EXT;
    ColorSpace ColorSpaces::extended_srgb_nonlinear = VK_COLOR_SPACE_EXTENDED_SRGB_NONLINEAR_EXT;
    ColorSpace ColorSpaces::display_native = VK_COLOR_SPACE_DISPLAY_NATIVE_AMD;
    ColorSpace ColorSpaces::max_enum = VK_COLOR_SPACE_MAX_ENUM_KHR;

    std::string ColorSpaces::ToString(ColorSpace colorSpace)
    {
        const static std::unordered_map<ColorSpace, std::string> colorSpaceNames =
        {
            { ColorSpaces::srgb_nonlinear, "srgb_nonlinear" },
            { ColorSpaces::display_p3_nonlinear, "display_p3_nonlinear" },
            { ColorSpaces::extended_srgb_linear, "extended_srgb_linear" },
            { ColorSpaces::display_p3_linear, "display_p3_linear" },
            { ColorSpaces::dci_p3_nonlinear, "dci_p3_nonlinear" },
            { ColorSpaces::bt709_linear, "bt709_linear" },
            { ColorSpaces::bt709_nonlinear, "bt709_nonlinear" },
            { ColorSpaces::bt2020_linear, "bt2020_linear" },
            { ColorSpaces::hdr10_st2084, "hdr10_st2084" },
            { ColorSpaces::dolbyvision, "dolbyvision" },
            { ColorSpaces::hdr10_hlg, "hdr10_hlg" },
            { ColorSpaces::adobergb_linear, "adobergb_linear" },
            { ColorSpaces::adobergb_nonlinear, "adobergb_nonlinear" },
            { ColorSpaces::pass_through, "pass_through" },
            { ColorSpaces::extended_srgb_nonlinear, "extended_srgb_nonlinear" },
            { ColorSpaces::display_native, "display_native" },
            { ColorSpaces::max_enum, "max_enum" }
        };

        auto it = colorSpaceNames.find(colorSpace);
        return it != colorSpaceNames.end() ? it->second : "unknown";
    }
}