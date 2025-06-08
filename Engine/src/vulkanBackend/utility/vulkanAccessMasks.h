#ifndef __INCLUDE_GUARD_vulkanAccessMasks_h__
#define __INCLUDE_GUARD_vulkanAccessMasks_h__
#include <string>
#include <vulkan/vulkan.h>



namespace emberEngine
{
	namespace vulkanBackend
	{
		namespace accessMask
		{
			namespace topOfPipe				// VK_PIPELINE_STAGE_2_TOP_OF_PIPE_BIT
			{
				constexpr VkAccessFlagBits2 none = VK_ACCESS_2_NONE;
			}

			namespace transfer
			{
				constexpr VkAccessFlagBits2 transferRead = VK_ACCESS_2_TRANSFER_READ_BIT;
				constexpr VkAccessFlagBits2 transferWrite = VK_ACCESS_2_TRANSFER_WRITE_BIT;
				constexpr VkAccessFlagBits2 none = VK_ACCESS_2_NONE;
			}

			namespace computeShader			// VK_PIPELINE_STAGE_2_COMPUTE_SHADER_BIT
			{
				constexpr VkAccessFlagBits2 uniformRead = VK_ACCESS_2_UNIFORM_READ_BIT;
				constexpr VkAccessFlagBits2 shaderRead = VK_ACCESS_2_SHADER_READ_BIT;
				constexpr VkAccessFlagBits2 shaderWrite = VK_ACCESS_2_SHADER_WRITE_BIT;
				constexpr VkAccessFlagBits2 sampledRead = VK_ACCESS_2_SHADER_SAMPLED_READ_BIT;
				constexpr VkAccessFlagBits2 storageRead = VK_ACCESS_2_SHADER_STORAGE_READ_BIT;
				constexpr VkAccessFlagBits2 storageWrite = VK_ACCESS_2_SHADER_STORAGE_WRITE_BIT;
				constexpr VkAccessFlagBits2 memoryRead = VK_ACCESS_2_MEMORY_READ_BIT;
				constexpr VkAccessFlagBits2 memoryWrite = VK_ACCESS_2_MEMORY_WRITE_BIT;
				constexpr VkAccessFlagBits2 accelerationStructureRead = VK_ACCESS_2_ACCELERATION_STRUCTURE_READ_BIT_KHR;
				constexpr VkAccessFlagBits2 descriptorBufferRead = VK_ACCESS_2_DESCRIPTOR_BUFFER_READ_BIT_EXT;
				constexpr VkAccessFlagBits2 none = VK_ACCESS_2_NONE;
			}

			namespace vertexAttributeInput	// VK_PIPELINE_STAGE_2_VERTEX_ATTRIBUTE_INPUT_BIT
			{
				constexpr VkAccessFlagBits2 vertexAttributeRead = VK_ACCESS_2_VERTEX_ATTRIBUTE_READ_BIT;
				constexpr VkAccessFlagBits2 none = VK_ACCESS_2_NONE;
			}

			namespace vertexInput			// VK_PIPELINE_STAGE_2_VERTEX_INPUT_BIT
			{
				constexpr VkAccessFlagBits2 indexRead = VK_ACCESS_2_INDEX_READ_BIT;
				constexpr VkAccessFlagBits2 vertexAttributeRead = VK_ACCESS_2_VERTEX_ATTRIBUTE_READ_BIT;
				constexpr VkAccessFlagBits2 none = VK_ACCESS_2_NONE;
			}

			namespace vertexShader			// VK_PIPELINE_STAGE_2_VERTEX_SHADER_BIT
			{
				constexpr VkAccessFlagBits2 uniformRead = VK_ACCESS_2_UNIFORM_READ_BIT;
				constexpr VkAccessFlagBits2 shaderRead = VK_ACCESS_2_SHADER_READ_BIT;
				constexpr VkAccessFlagBits2 shaderWrite = VK_ACCESS_2_SHADER_WRITE_BIT;
				constexpr VkAccessFlagBits2 sampledRead = VK_ACCESS_2_SHADER_SAMPLED_READ_BIT;
				constexpr VkAccessFlagBits2 storageRead = VK_ACCESS_2_SHADER_STORAGE_READ_BIT;
				constexpr VkAccessFlagBits2 storageWrite = VK_ACCESS_2_SHADER_STORAGE_WRITE_BIT;
				constexpr VkAccessFlagBits2 accelerationStructureRead = VK_ACCESS_2_ACCELERATION_STRUCTURE_READ_BIT_KHR;
				constexpr VkAccessFlagBits2 descriptorBufferRead = VK_ACCESS_2_DESCRIPTOR_BUFFER_READ_BIT_EXT;
				constexpr VkAccessFlagBits2 none = VK_ACCESS_2_NONE;
			}

			namespace earlyFragmentTest		// VK_PIPELINE_STAGE_2_EARLY_FRAGMENT_TESTS_BIT
			{
				constexpr VkAccessFlagBits2 depthStencilAttachmentRead = VK_ACCESS_2_DEPTH_STENCIL_ATTACHMENT_READ_BIT;
				constexpr VkAccessFlagBits2 depthStencilAttachmentWrite = VK_ACCESS_2_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;
				constexpr VkAccessFlagBits2 none = VK_ACCESS_2_NONE;
			}

			namespace fragmentShader		// VK_PIPELINE_STAGE_2_FRAGMENT_SHADER_BIT
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
				constexpr VkAccessFlagBits2 none = VK_ACCESS_2_NONE;
			}

			namespace colorAttachmentOutput	// VK_PIPELINE_STAGE_2_COLOR_ATTACHMENT_OUTPUT_BIT
			{
				constexpr VkAccessFlagBits2 colorAttachmentRead = VK_ACCESS_2_COLOR_ATTACHMENT_READ_BIT;
				constexpr VkAccessFlagBits2 colorAttachmentWrite = VK_ACCESS_2_COLOR_ATTACHMENT_WRITE_BIT;
				constexpr VkAccessFlagBits2 none = VK_ACCESS_2_NONE;
			}

			namespace copy					// VK_PIPELINE_STAGE_2_COPY_BIT
			{
				constexpr VkAccessFlagBits2 transferRead = VK_ACCESS_2_TRANSFER_READ_BIT;
				constexpr VkAccessFlagBits2 transferWrite = VK_ACCESS_2_TRANSFER_WRITE_BIT;
				constexpr VkAccessFlagBits2 none = VK_ACCESS_2_NONE;
			}

			namespace none					// VK_PIPELINE_STAGE_2_NONE
			{
				constexpr VkAccessFlagBits2 none = VK_ACCESS_2_NONE;
			}

			namespace bottomOfPipe			// VK_PIPELINE_STAGE_2_BOTTOM_OF_PIPE_BIT
			{
				constexpr VkAccessFlagBits2 none = VK_ACCESS_2_NONE;
			}
		}
	}
}



#endif // __INCLUDE_GUARD_vulkanAccessMasks_h__