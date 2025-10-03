#include "vulkanConvertAccessMask.h"
#include <unordered_map>
#include <vulkan/vulkan.h>



namespace vulkanRendererBackend
{
	vulkanRendererBackend::AccessMask AccessMaskCommonToVulkan(emberCommon::AccessMask accessMask)
	{
		switch (accessMask.flag)
		{
			case emberCommon::AccessMaskFlag::none: return VK_ACCESS_2_NONE;
			case emberCommon::AccessMaskFlag::transferRead: return VK_ACCESS_2_TRANSFER_READ_BIT;
			case emberCommon::AccessMaskFlag::transferWrite: return VK_ACCESS_2_TRANSFER_WRITE_BIT;
			case emberCommon::AccessMaskFlag::uniformRead: return VK_ACCESS_2_UNIFORM_READ_BIT;
			case emberCommon::AccessMaskFlag::shaderRead: return VK_ACCESS_2_SHADER_READ_BIT;
			case emberCommon::AccessMaskFlag::shaderWrite: return VK_ACCESS_2_SHADER_WRITE_BIT;
			case emberCommon::AccessMaskFlag::sampledRead: return VK_ACCESS_2_SHADER_SAMPLED_READ_BIT;
			case emberCommon::AccessMaskFlag::storageRead: return VK_ACCESS_2_SHADER_STORAGE_READ_BIT;
			case emberCommon::AccessMaskFlag::storageWrite: return VK_ACCESS_2_SHADER_STORAGE_WRITE_BIT;
			case emberCommon::AccessMaskFlag::memoryRead: return VK_ACCESS_2_MEMORY_READ_BIT;
			case emberCommon::AccessMaskFlag::memoryWrite: return VK_ACCESS_2_MEMORY_WRITE_BIT;
			case emberCommon::AccessMaskFlag::accelerationStructureRead: return VK_ACCESS_2_ACCELERATION_STRUCTURE_READ_BIT_KHR;
			case emberCommon::AccessMaskFlag::descriptorBufferRead: return VK_ACCESS_2_DESCRIPTOR_BUFFER_READ_BIT_EXT;
			case emberCommon::AccessMaskFlag::vertexAttributeRead: return VK_ACCESS_2_VERTEX_ATTRIBUTE_READ_BIT;
			case emberCommon::AccessMaskFlag::indexRead: return VK_ACCESS_2_INDEX_READ_BIT;
			case emberCommon::AccessMaskFlag::depthStencilAttachmentRead: return VK_ACCESS_2_DEPTH_STENCIL_ATTACHMENT_READ_BIT;
			case emberCommon::AccessMaskFlag::depthStencilAttachmentWrite: return VK_ACCESS_2_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;
			case emberCommon::AccessMaskFlag::colorAttachmentRead: return VK_ACCESS_2_COLOR_ATTACHMENT_READ_BIT;
			case emberCommon::AccessMaskFlag::colorAttachmentWrite: return VK_ACCESS_2_COLOR_ATTACHMENT_WRITE_BIT;
			default: return VK_ACCESS_2_NONE;
		}
	}
}