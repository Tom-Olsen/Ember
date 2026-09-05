#include "spirvImageFormatToVulkan.h"
#include <stdexcept>



namespace emberSpirvReflect
{
	VkFormat ImageFormatSpirvToVulkan(SpvImageFormat imageFormat)
	{
		switch (imageFormat)
		{
			case SpvImageFormatUnknown: return VK_FORMAT_UNDEFINED;
			case SpvImageFormatRgba32f: return VK_FORMAT_R32G32B32A32_SFLOAT;
			case SpvImageFormatRgba16f: return VK_FORMAT_R16G16B16A16_SFLOAT;
			case SpvImageFormatR32f: return VK_FORMAT_R32_SFLOAT;
			case SpvImageFormatRgba8: return VK_FORMAT_R8G8B8A8_UNORM;
			case SpvImageFormatRgba8Snorm: return VK_FORMAT_R8G8B8A8_SNORM;
			case SpvImageFormatRg32f: return VK_FORMAT_R32G32_SFLOAT;
			case SpvImageFormatRg16f: return VK_FORMAT_R16G16_SFLOAT;
			case SpvImageFormatR11fG11fB10f: return VK_FORMAT_B10G11R11_UFLOAT_PACK32;
			case SpvImageFormatR16f: return VK_FORMAT_R16_SFLOAT;
			case SpvImageFormatRgba16: return VK_FORMAT_R16G16B16A16_UNORM;
			case SpvImageFormatRgb10A2: return VK_FORMAT_A2B10G10R10_UNORM_PACK32;
			case SpvImageFormatRg16: return VK_FORMAT_R16G16_UNORM;
			case SpvImageFormatRg8: return VK_FORMAT_R8G8_UNORM;
			case SpvImageFormatR16: return VK_FORMAT_R16_UNORM;
			case SpvImageFormatR8: return VK_FORMAT_R8_UNORM;
			case SpvImageFormatRgba16Snorm: return VK_FORMAT_R16G16B16A16_SNORM;
			case SpvImageFormatRg16Snorm: return VK_FORMAT_R16G16_SNORM;
			case SpvImageFormatRg8Snorm: return VK_FORMAT_R8G8_SNORM;
			case SpvImageFormatR16Snorm: return VK_FORMAT_R16_SNORM;
			case SpvImageFormatR8Snorm: return VK_FORMAT_R8_SNORM;
			case SpvImageFormatRgba32i: return VK_FORMAT_R32G32B32A32_SINT;
			case SpvImageFormatRgba16i: return VK_FORMAT_R16G16B16A16_SINT;
			case SpvImageFormatRgba8i: return VK_FORMAT_R8G8B8A8_SINT;
			case SpvImageFormatR32i: return VK_FORMAT_R32_SINT;
			case SpvImageFormatRg32i: return VK_FORMAT_R32G32_SINT;
			case SpvImageFormatRg16i: return VK_FORMAT_R16G16_SINT;
			case SpvImageFormatRg8i: return VK_FORMAT_R8G8_SINT;
			case SpvImageFormatR16i: return VK_FORMAT_R16_SINT;
			case SpvImageFormatR8i: return VK_FORMAT_R8_SINT;
			case SpvImageFormatRgba32ui: return VK_FORMAT_R32G32B32A32_UINT;
			case SpvImageFormatRgba16ui: return VK_FORMAT_R16G16B16A16_UINT;
			case SpvImageFormatRgba8ui: return VK_FORMAT_R8G8B8A8_UINT;
			case SpvImageFormatR32ui: return VK_FORMAT_R32_UINT;
			case SpvImageFormatRgb10a2ui: return VK_FORMAT_A2B10G10R10_UINT_PACK32;
			case SpvImageFormatRg32ui: return VK_FORMAT_R32G32_UINT;
			case SpvImageFormatRg16ui: return VK_FORMAT_R16G16_UINT;
			case SpvImageFormatRg8ui: return VK_FORMAT_R8G8_UINT;
			case SpvImageFormatR16ui: return VK_FORMAT_R16_UINT;
			case SpvImageFormatR8ui: return VK_FORMAT_R8_UINT;
			case SpvImageFormatR64ui: return VK_FORMAT_R64_UINT;
			case SpvImageFormatR64i: return VK_FORMAT_R64_SINT;
			default: throw std::invalid_argument("ImageFormatSpirvToVulkan(...) failed. Unsupported SpvImageFormat: " + std::to_string(static_cast<uint32_t>(imageFormat)) + ".");
		}
	}
}