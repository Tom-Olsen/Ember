#include <string>
#include <vulkan/vulkan.h>



namespace emberVulkanUtility
{
	inline std::string ToString(VkVertexInputRate vkVertexInputRate)
	{
		switch (vkVertexInputRate)
		{
			case VK_VERTEX_INPUT_RATE_VERTEX:
				return "VK_VERTEX_INPUT_RATE_VERTEX(0)"; break;
			case VK_VERTEX_INPUT_RATE_INSTANCE:
				return "VK_VERTEX_INPUT_RATE_INSTANCE(1)"; break;
			case VK_VERTEX_INPUT_RATE_MAX_ENUM:
				return "VK_VERTEX_INPUT_RATE_MAX_ENUM"; break;
			default:
				return "unknown";
		}
	}
}