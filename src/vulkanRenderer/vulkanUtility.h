#ifndef __INCLUDE_GUARD_vulkanUtility_h__
#define __INCLUDE_GUARD_vulkanUtility_h__
#include <string>
#include <vulkan/vulkan.h>



namespace emberEngine
{
	namespace vulkanObjToString
	{
		std::string VkVertexInputAttributeDescriptionToString(VkVertexInputAttributeDescription vkVertexInputAttributeDescription);
		std::string VkImageViewTypeToString(VkImageViewType vkImageViewType);
		std::string VkImageLayoutToString(VkImageLayout vkImageView);
		std::string VkVertexInputRateToString(VkVertexInputRate vkVertexInputRate);
		std::string VkDescriptorTypeToString(VkDescriptorType vkDescriptorType);
		std::string VkFormatToString(VkFormat vkFormat);
		std::string VkShaderStageFlagsToString(VkShaderStageFlags vkShaderStageFlags);
	}

	// Add more Stages when needed: https://registry.khronos.org/vulkan/specs/latest/man/html/VkAccessFlagBits.html
	namespace AccessMask
	{
		namespace ComputeShader
		{
			constexpr VkAccessFlagBits2 uniformRead = VK_ACCESS_2_UNIFORM_READ_BIT;
			constexpr VkAccessFlagBits2 shaderRead = VK_ACCESS_2_SHADER_READ_BIT;
			constexpr VkAccessFlagBits2 shaderWrite = VK_ACCESS_2_SHADER_WRITE_BIT;
			constexpr VkAccessFlagBits2 sampledRead = VK_ACCESS_2_SHADER_SAMPLED_READ_BIT;
			constexpr VkAccessFlagBits2 storageRead = VK_ACCESS_2_SHADER_STORAGE_READ_BIT;
			constexpr VkAccessFlagBits2 storageWrite = VK_ACCESS_2_SHADER_STORAGE_WRITE_BIT;
			constexpr VkAccessFlagBits2 accelerationStructureRead = VK_ACCESS_2_ACCELERATION_STRUCTURE_READ_BIT_KHR;
			constexpr VkAccessFlagBits2 descriptorBufferRead = VK_ACCESS_2_DESCRIPTOR_BUFFER_READ_BIT_EXT;
		}

		namespace VertexAttributeInput
		{
			constexpr VkAccessFlagBits2 vertexAttributeRead = VK_ACCESS_2_VERTEX_ATTRIBUTE_READ_BIT;
		}

		namespace VertexInput
		{
			constexpr VkAccessFlagBits2 indexRead = VK_ACCESS_2_INDEX_READ_BIT;
			constexpr VkAccessFlagBits2 vertexAttributeRead = VK_ACCESS_2_VERTEX_ATTRIBUTE_READ_BIT;
		}

		namespace VertexShader
		{
			constexpr VkAccessFlagBits2 uniformRead = VK_ACCESS_2_UNIFORM_READ_BIT;
			constexpr VkAccessFlagBits2 shaderRead = VK_ACCESS_2_SHADER_READ_BIT;
			constexpr VkAccessFlagBits2 shaderWrite = VK_ACCESS_2_SHADER_WRITE_BIT;
			constexpr VkAccessFlagBits2 sampledRead = VK_ACCESS_2_SHADER_SAMPLED_READ_BIT;
			constexpr VkAccessFlagBits2 storageRead = VK_ACCESS_2_SHADER_STORAGE_READ_BIT;
			constexpr VkAccessFlagBits2 storageWrite = VK_ACCESS_2_SHADER_STORAGE_WRITE_BIT;
			constexpr VkAccessFlagBits2 accelerationStructureRead = VK_ACCESS_2_ACCELERATION_STRUCTURE_READ_BIT_KHR;
			constexpr VkAccessFlagBits2 descriptorBufferRead = VK_ACCESS_2_DESCRIPTOR_BUFFER_READ_BIT_EXT;
		}

		namespace FragmentShader
		{
			constexpr VkAccessFlagBits2 uniformRead = VK_ACCESS_2_UNIFORM_READ_BIT;
			constexpr VkAccessFlagBits2 shaderRead = VK_ACCESS_2_SHADER_READ_BIT;
			constexpr VkAccessFlagBits2 shaderWrite = VK_ACCESS_2_SHADER_WRITE_BIT;
			constexpr VkAccessFlagBits2 colorAttachmentRead = VK_ACCESS_2_COLOR_ATTACHMENT_READ_BIT;
			constexpr VkAccessFlagBits2 depthStencilAttachmentRead = VK_ACCESS_2_DEPTH_STENCIL_ATTACHMENT_READ_BIT;
			constexpr VkAccessFlagBits2 sampledRead = VK_ACCESS_2_SHADER_SAMPLED_READ_BIT;
			constexpr VkAccessFlagBits2 storageRead = VK_ACCESS_2_SHADER_STORAGE_READ_BIT;
			constexpr VkAccessFlagBits2 storageWrite = VK_ACCESS_2_SHADER_STORAGE_WRITE_BIT;
			constexpr VkAccessFlagBits2 accelerationStructureRead = VK_ACCESS_2_ACCELERATION_STRUCTURE_READ_BIT_KHR;
			constexpr VkAccessFlagBits2 descriptorBufferRead = VK_ACCESS_2_DESCRIPTOR_BUFFER_READ_BIT_EXT;
		}

		namespace ColorAttachmentOutput
		{
			constexpr VkAccessFlagBits2 colorAttachmentRead = VK_ACCESS_2_COLOR_ATTACHMENT_READ_BIT;
			constexpr VkAccessFlagBits2 colorAttachmentWrite = VK_ACCESS_2_COLOR_ATTACHMENT_WRITE_BIT;
		}
	}
}



#endif // __INCLUDE_GUARD_vulkanUtility_h__