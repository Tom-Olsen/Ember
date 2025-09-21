#include "vulkanIndexType.h"
#include <unordered_map>
#include <vulkan/vulkan.h>



namespace vulkanRendererBackend
{
	IndexType IndexTypes::uint16 = VK_INDEX_TYPE_UINT16;
	IndexType IndexTypes::uint32 = VK_INDEX_TYPE_UINT32;
	IndexType IndexTypes::uint8 = VK_INDEX_TYPE_UINT8;
	IndexType IndexTypes::none_khr = VK_INDEX_TYPE_NONE_KHR;
	IndexType IndexTypes::none_nv = VK_INDEX_TYPE_NONE_NV;
	IndexType IndexTypes::uint8_ext = VK_INDEX_TYPE_UINT8_EXT;
	IndexType IndexTypes::uint8_khr = VK_INDEX_TYPE_UINT8_KHR;
	IndexType IndexTypes::max_enum = VK_INDEX_TYPE_MAX_ENUM;

	std::string IndexTypes::ToString(IndexType indexType)
	{
        const static std::unordered_map<IndexType, std::string> indexTypeNames =
        {
            { IndexTypes::uint16, "uint16" },
            { IndexTypes::uint32, "uint32" },
            { IndexTypes::uint8, "uint8" },
            { IndexTypes::none_khr, "none_khr" },
            { IndexTypes::none_nv, "none_nv" },
            { IndexTypes::uint8_ext, "uint8_ext" },
            { IndexTypes::uint8_khr, "uint8_khr" },
            { IndexTypes::max_enum, "max_enum" }
        };

        auto it = indexTypeNames.find(indexType);
        return it != indexTypeNames.end() ? it->second : "unknown";
	}
}